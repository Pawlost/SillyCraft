// Fill out your copyright notice in the Description page of Project Settings.

#include "Meshers/RunLengthMesher.h"

#include "RealtimeMeshSimple.h"
#include "Mesh/RealtimeMeshBuilder.h"

const URunLengthMesher::FNormalsAndTangents URunLengthMesher::FaceNormalsAndTangents[] = {
	{FVector3f(-1.0f, 0.0f, 0.0f), FVector3f(0.0, 1.0, 0.0)}, //Front
	{FVector3f(1.0f, 0.0f, 0.0f), FVector3f(0.0, 1.0, 0.0)}, //Back
	{FVector3f(0.0f, -1.0f, 0.0f), FVector3f(1.0f, 0.0f, 0.0f)}, // Right 
	{FVector3f(0.0f, 1.0f, 0.0f), FVector3f(1.0, 0.0, 0.0)}, // Left
	{FVector3f(0.0f, 0.0f, -1.0f), FVector3f(1.0f, 0.0f, 0.0f)}, //Bottom
	{FVector3f(0.0f, 0.0f, 1.0f), FVector3f(1.0f, 0.0f, 0.0f)} //Top
};

void URunLengthMesher::UpdateAllFacesParams()
{
	auto chunkLenght = VoxelGenerator->GetVoxelDimensionCount();
	//Axis X
	UpdateFaceParams(FrontFaceTemplate, FIntVector(-1, 0, 0),
	                 FIntVector(chunkLenght - 1, 0, 0),
	                 FIntVector(0, -1, 0));

	UpdateFaceParams(BackFaceTemplate, FIntVector(1, 0, 0),
	                 FIntVector(0, 0, 0),
	                 FIntVector(0, -1, 0));

	//Axis Y
	UpdateFaceParams(RightFaceTemplate, FIntVector(0, -1, 0),
	                 FIntVector(0, chunkLenght - 1, 0),
	                 FIntVector(-1, 0, 0));

	UpdateFaceParams(LeftFaceTemplate, FIntVector(0, 1, 0),
	                 FIntVector(0, 0, 0),
	                 FIntVector(-1, 0, 0));

	// Axis Z
	UpdateFaceParams(BottomFaceTemplate, FIntVector(0, 0, -1),
	                 FIntVector(0, 0, chunkLenght - 1),
	                 FIntVector(0, -1, 0));

	UpdateFaceParams(TopFaceTemplate, FIntVector(0, 0, 1),
	                 FIntVector(0, 0, 0),
	                 FIntVector(0, -1, 0));
}

void URunLengthMesher::UpdateFaceParams(FNaiveMeshingData& face, FIntVector forwardVoxelIndexVector,
                                        FIntVector chunkBorderIndexVector,
                                        FIntVector previousVoxelIndexVector) const
{
	face.ForwardVoxelIndex = VoxelGenerator->GetVoxelIndex(forwardVoxelIndexVector);
	face.PreviousVoxelIndex = VoxelGenerator->GetVoxelIndex(previousVoxelIndexVector);
	face.ChunkBorderIndex = VoxelGenerator->GetVoxelIndex(chunkBorderIndexVector);
}

void URunLengthMesher::GenerateMesh(FChunkFaceParams& faceParams)
{
	if (faceParams.ChunkParams.OriginalChunk->IsEmpty)
	{
		faceParams.ChunkParams.OriginalChunk->ChunkMeshActor->RealtimeMeshComponent->SetVisibility(false);
		return;
	}
	
	faceParams.ChunkParams.OriginalChunk->ChunkMeshActor->RealtimeMeshComponent->SetVisibility(true);

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation")
#endif

	// Voxel faces need to be hidden under multiple layers of abstraction because Realtime Mesh Component requires it

	InitFaceContainers(faceParams);
	FaceGeneration(faceParams);
	DirectionalGreedyMeshing(faceParams);
	GenerateMeshFromFaces(faceParams);
}

void URunLengthMesher::InitFaceContainers(FChunkFaceParams& faceParams) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation intialization")
#endif

	int32 chunkPlane = VoxelGenerator->GetVoxel3DimensionCount();
	for (int f = 0; f < FACE_SIDE_COUNT; f++)
	{
		faceParams.Faces[f].SetNum(faceParams.ChunkParams.OriginalChunk->ChunkVoxelTypeTable.Num());
		for (auto voxelId : faceParams.ChunkParams.OriginalChunk->ChunkVoxelTypeTable)
		{
			auto faceArray = MakeShared<TArray<FChunkFace>>();
			faceArray->Reserve(chunkPlane);
			faceParams.Faces[f][voxelId.Value.ChunkVoxelId] = faceArray;
		}
	}
}

void URunLengthMesher::FaceGeneration(FChunkFaceParams& faceParams) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Run lenght meshing generation")
#endif

	auto chunkLenght = VoxelGenerator->GetVoxelDimensionCount();
	// culling and run meshing
	for (int32 x = 0; x < chunkLenght; x++)
	{
		auto minBorder = IsMinBorder(x);
		auto maxBorder = IsMaxBorder(x);

		auto xAxisIndex = VoxelGenerator->GetVoxelIndex(x, 0, 0);
		auto yAxisIndex = VoxelGenerator->GetVoxelIndex(0, x, 0);
		auto zAxisIndex = VoxelGenerator->GetVoxelIndex(0, 0, x);

		for (int32 z = 0; z < chunkLenght; z++)
		{
			for (int32 y = 0; y < chunkLenght; y++)
			{
				IncrementRun(x, y, z, xAxisIndex, minBorder, maxBorder, FrontFaceTemplate, BackFaceTemplate,
				             faceParams);
				IncrementRun(y, x, z, yAxisIndex, minBorder, maxBorder, RightFaceTemplate, LeftFaceTemplate,
				             faceParams);
				IncrementRun(z, y, x, zAxisIndex, minBorder, maxBorder, BottomFaceTemplate, TopFaceTemplate,
				             faceParams);
			}
		}
	}
}

void URunLengthMesher::IncrementRun(int x, int y, int z, int32 axisVoxelIndex, bool isMinBorder, bool isMaxBorder,
                                    const FNaiveMeshingData& faceTemplate,
                                    const FNaiveMeshingData& reversedFaceTemplate,
                                    FChunkFaceParams& chunkParams) const
{
	auto index = VoxelGenerator->GetVoxelIndex(x, y, z);
	auto voxel = chunkParams.ChunkParams.OriginalChunk->Voxels[index];

	if (!voxel.IsEmptyVoxel())
	{
		auto position = FIntVector(x, y, z);
		auto sideChunk = chunkParams.ChunkParams.OriginalChunk;
		if (!sideChunk.IsValid())
		{
			return;
		}
		auto voxelId = sideChunk->ChunkVoxelTypeTable[voxel.VoxelId];
		auto faceContainerIndex = static_cast<uint8>(faceTemplate.StaticMeshingData.faceSide);
		auto faceContainerVoxelIndex = static_cast<uint8>(reversedFaceTemplate.StaticMeshingData.faceSide);
		AddFace(faceTemplate, isMinBorder, index, position, voxel, axisVoxelIndex,
		        chunkParams.Faces[faceContainerIndex][voxelId.ChunkVoxelId], chunkParams.ChunkParams);
		AddFace(reversedFaceTemplate, isMaxBorder, index, position, voxel, axisVoxelIndex,
		        chunkParams.Faces[faceContainerVoxelIndex][voxelId.ChunkVoxelId], chunkParams.ChunkParams);
	}
}

void URunLengthMesher::AddFace(const FNaiveMeshingData& faceTemplate, bool isBorder,
                               const int32& index, const FIntVector& position, const FVoxel& voxel,
                               const int32& axisVoxelIndex,
                               const TSharedPtr<TArray<FChunkFace>>& chunkFaces, const FChunkParams& chunkParams)
{
	const FVoxelIndexParams voxelIndexParams =
	{
		isBorder,
		faceTemplate.ForwardVoxelIndex + index,
		faceTemplate.PreviousVoxelIndex + index,
		index - axisVoxelIndex + faceTemplate.ChunkBorderIndex,
		faceTemplate.StaticMeshingData.faceSide
	};

	if (IsBorderVoxelVisible(voxelIndexParams, chunkParams) || IsVoxelVisible(voxelIndexParams, chunkParams))
	{
		FChunkFace newFace =
		{
			voxel,
			faceTemplate.StaticMeshingData.faceTemplate.StartVertexDown + position,
			faceTemplate.StaticMeshingData.faceTemplate.EndVertexDown + position,
			faceTemplate.StaticMeshingData.faceTemplate.EndVertexUp + position,
			faceTemplate.StaticMeshingData.faceTemplate.StartVertexUp + position,
		};

		if (!chunkFaces->IsEmpty())
		{
			FChunkFace& prevFace = chunkFaces->Last();
			if (faceTemplate.StaticMeshingData.naiveFaceMerge(prevFace, newFace))
			{
				//return when new face was merged
				return;
			}
		}

		chunkFaces->Push(newFace);
	}
}

bool URunLengthMesher::IsBorderVoxelVisible(const FVoxelIndexParams& faceData, const FChunkParams& chunkStruct)
{
	if (faceData.IsBorder)
	{
		auto faceContainerIndex = static_cast<uint8>(faceData.FaceDirection);
		auto sideChunk = chunkStruct.SideChunks[faceContainerIndex];
		return sideChunk == nullptr || sideChunk->Voxels[faceData.CurrentVoxelIndex].IsEmptyVoxel();
	}
	return false;
}

bool URunLengthMesher::IsVoxelVisible(const FVoxelIndexParams& faceData, const FChunkParams& chunkStruct)
{
	return !faceData.IsBorder && chunkStruct.OriginalChunk->Voxels.IsValidIndex(faceData.ForwardVoxelIndex) &&
		chunkStruct.OriginalChunk->Voxels[faceData.ForwardVoxelIndex].IsEmptyVoxel();
}

void URunLengthMesher::GenerateVoxels(TSharedPtr<FChunkStruct>& chunk)
{
	VoxelGenerator->GenerateVoxels(chunk);
}

void URunLengthMesher::BeginPlay()
{
	if (VoxelGeneratorClass)
	{
		// Create the component dynamically
		VoxelGenerator = NewObject<UVoxelGeneratorBase>(this, VoxelGeneratorClass);

		if (VoxelGenerator)
		{
			VoxelGenerator->RegisterComponent();
		}
	}
	UpdateAllFacesParams();

	Super::BeginPlay();
}

void URunLengthMesher::DirectionalGreedyMeshing(const FChunkFaceParams& faceParams)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Run directional greedy mesh generation")
#endif

	for (int8 f = 0; f < FACE_SIDE_COUNT; f++)
	{
		for (auto voxelId : faceParams.ChunkParams.OriginalChunk->ChunkVoxelTypeTable)
		{
			auto faceContainer = faceParams.Faces[f][voxelId.Value.ChunkVoxelId];
			auto lastElementIndex = faceContainer->Num() - 1;

			for (int32 i = lastElementIndex - 1; i >= 0; i--)
			{
				FChunkFace& face = (*faceContainer)[i];
				FChunkFace& nextFace = (*faceContainer)[i + 1];

				if (FChunkFace::MergeFaceUp(face, nextFace))
				{
					faceContainer->RemoveAt(i + 1, EAllowShrinking::No);
				}
			}
		}
	}
}

void URunLengthMesher::GenerateMeshFromFaces(const FChunkFaceParams& faceParams) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh stream generation")
#endif

	auto StreamSet = MakeShared<FRealtimeMeshStreamSet>();

	TRealtimeMeshBuilderLocal<int32> Builder(*StreamSet.ToWeakPtr().Pin());

	Builder.EnableTexCoords();
	Builder.EnableColors();
	Builder.EnableTangents();

	Builder.EnablePolyGroups();

	if (!IsValid(VoxelGenerator))
	{
		return;
	}

	auto voxelSize = VoxelGenerator->GetVoxelSize();

	TMap<uint32, uint16> voxelIdsInMesh;

	// Because of RealTimeMesh component voxelId needs to be first
	for (auto voxelId : faceParams.ChunkParams.OriginalChunk->ChunkVoxelTypeTable)
	{
		for (int faceIndex = 0; faceIndex < FACE_SIDE_COUNT; faceIndex++)
		{
			auto faceContainer = faceParams.Faces[faceIndex];

			auto sideFaces = faceContainer[voxelId.Value.ChunkVoxelId];

			auto faceNormalAndTangent = FaceNormalsAndTangents[faceIndex];
			for (auto Face : *sideFaces)
			{
				// Add our first vertex
				int32 V0 = Builder.AddVertex(Face.GetFinalStartVertexDown(voxelSize))
				                  .SetColor(FColor::White)
				                  .SetTexCoord(FVector2f(0, 0))
				                  .SetNormalAndTangent(faceNormalAndTangent.Normal, faceNormalAndTangent.Tangent);

				// Add our second vertex
				int32 V1 = Builder.AddVertex(Face.GetFinalEndVertexDown(voxelSize))
				                  .SetColor(FColor::White)
				                  .SetTexCoord(FVector2f(1, 0))
				                  .SetNormalAndTangent(faceNormalAndTangent.Normal, faceNormalAndTangent.Tangent);

				// Add our third vertex
				int32 V2 = Builder.AddVertex(Face.GetFinalEndVertexUp(voxelSize))
				                  .SetColor(FColor::White)
				                  .SetTexCoord(FVector2f(1, 1))
				                  .SetNormalAndTangent(faceNormalAndTangent.Normal, faceNormalAndTangent.Tangent);

				int32 V3 = Builder.AddVertex(Face.GetFinalStartVertexUp(voxelSize))
				                  .SetColor(FColor::White)
				                  .SetTexCoord(FVector2f(0, 1))
				                  .SetNormalAndTangent(faceNormalAndTangent.Normal, faceNormalAndTangent.Tangent);

				if (!voxelIdsInMesh.Contains(voxelId.Key))
				{
					voxelIdsInMesh.Add(voxelId.Key, voxelIdsInMesh.Num());
				}

				Builder.AddTriangle(V0, V1, V2, voxelIdsInMesh[voxelId.Key]);
				Builder.AddTriangle(V2, V3, V0, voxelIdsInMesh[voxelId.Key]);
			}
		}
	}

	if (!faceParams.ChunkParams.OriginalChunk.IsValid() || !faceParams.ChunkParams.OriginalChunk->ChunkMeshActor.
	                                                                   IsValid())
	{
		return;
	}

	auto RealtimeMesh = faceParams.ChunkParams.OriginalChunk->ChunkMeshActor->RealtimeMeshComponent->GetRealtimeMeshAs<
		URealtimeMeshSimple>();

	const FRealtimeMeshSectionGroupKey GroupKey = FRealtimeMeshSectionGroupKey::Create(0, FName("Chunk Mesh"));

	// Now we create the section group, since the stream set has polygroups, this will create the sections as well
	RealtimeMesh->CreateSectionGroup(GroupKey, *StreamSet);

	for (auto voxelId : voxelIdsInMesh)
	{
		auto materialId = voxelId.Value;
		FVoxelType voxelType = VoxelGenerator->GetVoxelTypeById(voxelId.Key);
		RealtimeMesh->SetupMaterialSlot(materialId, voxelType.BlockName, voxelType.Material);

		auto key = FRealtimeMeshSectionKey::CreateForPolyGroup(GroupKey, materialId);
		RealtimeMesh->UpdateSectionConfig(key, FRealtimeMeshSectionConfig(
			                                  ERealtimeMeshSectionDrawType::Static, materialId),
		                                  true);
	}

	faceParams.ChunkParams.OriginalChunk->HasMesh = true;
}

double URunLengthMesher::GetChunkSize()
{
	return VoxelGenerator->GetChunkSize();
}

double URunLengthMesher::GetVoxelSize()
{
	return VoxelGenerator->GetVoxelSize();
}

bool URunLengthMesher::IsMinBorder(const int x)
{
	return x == 0;
}

bool URunLengthMesher::IsMaxBorder(const int x) const
{
	return x == VoxelGenerator->GetVoxelDimensionCount() - 1;
}

double URunLengthMesher::GetHighestElevationAtLocation(const FVector& location)
{
	return VoxelGenerator->GetHighestElevationAtLocation(location);
}

bool URunLengthMesher::ChangeVoxelIdInChunk(const TSharedPtr<FChunkStruct>& chunk, const FIntVector& voxelPosition,
                                            const FVoxel& voxelId)
{
	auto index = VoxelGenerator->GetVoxelIndex(voxelPosition);

	if (chunk.IsValid() && chunk->Voxels.IsValidIndex(index))
	{
		if (voxelId.IsEmptyVoxel())
		{
			FVoxel removedVoxel = chunk->Voxels[index];

			if (!removedVoxel.IsEmptyVoxel())
			{
				auto voxelInChunkPtr = chunk->ChunkVoxelTypeTable.Find(removedVoxel.VoxelId);
				voxelInChunkPtr->CountInChunk--;

				if (voxelInChunkPtr->CountInChunk <= 0)
				{
					chunk->ChunkVoxelTypeTable.Remove(removedVoxel.VoxelId);
					if (chunk->ChunkVoxelTypeTable.IsEmpty())
					{
						chunk->IsEmpty = true;
					}
					else
					{
						int voxelIndex = 0;
						TArray<int32> voxelKeys;
						chunk->ChunkVoxelTypeTable.GetKeys(voxelKeys);
						for (auto key : voxelKeys)
						{
							chunk->ChunkVoxelTypeTable[key].ChunkVoxelId = voxelIndex;
							voxelIndex++;
						}
					}
				}
			}

			chunk->Voxels[index] = voxelId;
		}
		else
		{
			VoxelGenerator->AddVoxelAtIndex(chunk, index, voxelId);
		}
		return true;
	}

	return false;
}
