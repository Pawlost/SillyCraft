﻿// Fill out your copyright notice in the Description page of Project Settings.

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

// TODO: optimize
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
	faceParams.ChunkParams.OriginalChunk->HasMesh = false;
	if (faceParams.ChunkParams.OriginalChunk->ChunkVoxelTypeTable.IsEmpty())
	{
		faceParams.ChunkParams.OriginalChunk->ChunkMeshActor->ClearMesh();
		return;
	}

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation")
#endif

	// Voxel faces need to be hidden under multiple layers of abstraction because Realtime Mesh Component requires it

	InitFaceContainers(faceParams);
	FaceGeneration(faceParams);
	DirectionalGreedyMeshing(faceParams);
	GenerateMeshFromFaces(faceParams);
}

void URunLengthMesher::VoxelGeneratorSet()
{
	UpdateAllFacesParams();
}

void URunLengthMesher::InitFaceContainers(FChunkFaceParams& faceParams) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation intialization")
#endif

	int32 chunkPlane = VoxelGenerator->GetVoxel3DimensionCount();
	faceParams.VoxelIdToLocalVoxelMap.Reserve(faceParams.ChunkParams.OriginalChunk->ChunkVoxelTypeTable.Num());


	for (auto voxelId : faceParams.ChunkParams.OriginalChunk->ChunkVoxelTypeTable)
	{
		auto localVoxelId = faceParams.VoxelIdToLocalVoxelMap.Num();
		faceParams.VoxelIdToLocalVoxelMap.Add(voxelId.Key, localVoxelId);
	}
	
	for (int f = 0; f < FACE_SIDE_COUNT; f++)
	{
		for (auto voxel : faceParams.VoxelIdToLocalVoxelMap)
		{
			faceParams.Faces[f].SetNum(faceParams.ChunkParams.OriginalChunk->ChunkVoxelTypeTable.Num());
			auto faceArray = MakeShared<TArray<FChunkFace>>();
			faceArray->Reserve(chunkPlane);
			faceParams.Faces[f][voxel.Value] = faceArray;
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
		auto originalChunk = chunkParams.ChunkParams.OriginalChunk;
		if (!originalChunk.IsValid())
		{
			return;
		}
		auto localVoxelId = chunkParams.VoxelIdToLocalVoxelMap[voxel.VoxelId];
		auto faceContainerIndex = static_cast<uint8>(faceTemplate.StaticMeshingData.faceSide);
		auto faceContainerVoxelIndex = static_cast<uint8>(reversedFaceTemplate.StaticMeshingData.faceSide);
		AddFace(faceTemplate, isMinBorder, index, position, voxel, axisVoxelIndex,
		        chunkParams.Faces[faceContainerIndex][localVoxelId], chunkParams.ChunkParams);
		AddFace(reversedFaceTemplate, isMaxBorder, index, position, voxel, axisVoxelIndex,
		        chunkParams.Faces[faceContainerVoxelIndex][localVoxelId], chunkParams.ChunkParams);
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

// TODO: optimize by skipping Z rows
void URunLengthMesher::DirectionalGreedyMeshing(const FChunkFaceParams& faceParams)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Run directional greedy mesh generation")
#endif

	for (int8 f = 0; f < FACE_SIDE_COUNT; f++)
	{
		for (auto voxelId : faceParams.VoxelIdToLocalVoxelMap)
		{
			auto faceContainer = faceParams.Faces[f][voxelId.Value];
			auto lastElementIndex = faceContainer->Num() - 1;

			for (int32 i = lastElementIndex - 1; i >= 0; i--)
			{
				FChunkFace& nextFace = (*faceContainer)[i + 1];

				int backTrackIndex = i;
				while (faceContainer->IsValidIndex(backTrackIndex))
				{
					FChunkFace& face = (*faceContainer)[backTrackIndex];

					if (face.StartVertexUp.Z < nextFace.StartVertexDown.Z)
					{
						break;
					}

					if (FChunkFace::MergeFaceUp(face, nextFace))
					{
						faceContainer->RemoveAt(i + 1, EAllowShrinking::No);
						break;
					}

					backTrackIndex--;
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
	for (auto voxelId : faceParams.VoxelIdToLocalVoxelMap)
	{
		for (int faceIndex = 0; faceIndex < FACE_SIDE_COUNT; faceIndex++)
		{
			auto faceContainer = faceParams.Faces[faceIndex];

			auto sideFaces = faceContainer[voxelId.Value];

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

	auto RMCActor = faceParams.ChunkParams.OriginalChunk->ChunkMeshActor;
	RMCActor->PrepareMesh();
	auto RealtimeMesh = RMCActor->RealtimeMeshComponent->GetRealtimeMeshAs<
		URealtimeMeshSimple>();
	
	// Now we create the section group, since the stream set has polygroups, this will create the sections as well
	RealtimeMesh->CreateSectionGroup(RMCActor->GroupKey, *StreamSet);

	for (auto voxelId : voxelIdsInMesh)
	{
		auto materialId = voxelId.Value;
		FVoxelType voxelType = VoxelGenerator->GetVoxelTypeById(voxelId.Key);
		RealtimeMesh->SetupMaterialSlot(materialId, voxelType.BlockName, voxelType.Material);

		auto key = FRealtimeMeshSectionKey::CreateForPolyGroup(RMCActor->GroupKey, materialId);
		RealtimeMesh->UpdateSectionConfig(key, FRealtimeMeshSectionConfig(
											  ERealtimeMeshSectionDrawType::Static, materialId),
										  true);
	}
	
	faceParams.ChunkParams.OriginalChunk->HasMesh = true;
}

bool URunLengthMesher::IsMinBorder(const int x)
{
	return x == 0;
}

bool URunLengthMesher::IsMaxBorder(const int x) const
{
	return x == VoxelGenerator->GetVoxelDimensionCount() - 1;
}
