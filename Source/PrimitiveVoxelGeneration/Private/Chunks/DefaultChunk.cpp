// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunks/DefaultChunk.h"

#include "FastNoiseWrapper.h"
#include "Chunks/ChunkGridData.h"
#include "Chunks/ChunkSettings.h"
#include "Chunks/MeshingStructs/StaticNaiveMeshingData.h"
#include "Voxels/Voxel.h"
#include "RealtimeMeshSimple.h"
#include "Voxels/VoxelType.h"

void UDefaultChunk::AddToGrid(const TWeakObjectPtr<UChunkGridData> chunkGridData, FIntVector& chunkGridPos)
{
	ChunkSettings = chunkGridData->GetChunkSettings();
	ChunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	Voxels.SetNum(ChunkLenght * ChunkSettings->GetChunkPlaneSizeInVoxels());
	UpdateAllFacesParams();
	Super::AddToGrid(chunkGridData, chunkGridPos);
}

void UDefaultChunk::UpdateAllFacesParams()
{
	//Axis X
	UpdateFaceParams(FrontFaceTemplate, FIntVector(-1,0,0),
	FIntVector(ChunkLenght - 1, 0, 0),
	FIntVector(0,-1,0));
	
	UpdateFaceParams(BackFaceTemplate, FIntVector(1,0,0),
	FIntVector(0, 0, 0),
	FIntVector(0,-1,0));

	//Axis Y
	UpdateFaceParams(RightFaceTemplate, FIntVector(0,-1,0),
	FIntVector(0, ChunkLenght - 1, 0),
	FIntVector(-1,0,0));
	
	UpdateFaceParams(LeftFaceTemplate, FIntVector(0,1,0),
	FIntVector(0, 0, 0),
	FIntVector(-1,0,0));
	
	// Axis Z
	UpdateFaceParams(BottomFaceTemplate, FIntVector(0,0,-1),
	FIntVector(0, 0, ChunkLenght - 1),
	FIntVector(0,-1,0));

	UpdateFaceParams(TopFaceTemplate, FIntVector(0,0,1),
	FIntVector(0, 0, 0),
	FIntVector(0,-1,0));
}

void UDefaultChunk::UpdateFaceParams(FNaiveMeshingData& face, FIntVector forwardVoxelIndexVector,
	FIntVector chunkBorderIndexVector,
	FIntVector previousVoxelIndexVector) const
{
	face.ForwardVoxelIndex = ChunkSettings->GetVoxelIndex(forwardVoxelIndexVector);
	face.PreviousVoxelIndex = ChunkSettings->GetVoxelIndex(previousVoxelIndexVector);
	face.ChunkBorderIndex = ChunkSettings->GetVoxelIndex(chunkBorderIndexVector);
}

UDefaultChunk::UDefaultChunk()
{
	Noise = CreateDefaultSubobject<UFastNoiseWrapper>("NoiseGenerator");
}

void UDefaultChunk::GenerateMesh()
{
	if(Empty)
	{
		return;
	}

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation")
#endif

	// Voxel faces need to be hidden under multiple layers of abstraction because Realtime Mesh Component requires it
	TArray<TSharedPtr<TArray<FChunkFace>>> faces[FACE_COUNT];
	
	InitFaceContainers(faces);
	FaceGeneration(faces);
	GreedyMeshAllFaces(faces);
	GenerateMeshFromFaces(faces);
}

void UDefaultChunk::InitFaceContainers(TArray<TSharedPtr<TArray<FChunkFace>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation intialization")
#endif
	
	int32 chunkPlane = ChunkSettings->GetChunkPlaneSizeInVoxels();
	for(int f = 0; f < FACE_COUNT; f++)
	{
		faces[f].SetNum(voxelIdsInMesh.Num());
		
		for (auto voxelId : voxelIdsInMesh)
		{
			auto faceArray = MakeShared<TArray<FChunkFace>>();
			faceArray->Reserve(chunkPlane);
			faces[f][voxelId.Value] = faceArray;
		}
	}
}

void UDefaultChunk::FaceGeneration(const TArray<TSharedPtr<TArray<FChunkFace>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Naive greedy mesh generation")
#endif
	
	// culling and naive greedy meshing
	for(int x = 0; x < ChunkLenght; x++)
	{
		auto minBorder = IsMinBorder(x);
		auto maxBorder = IsMaxBorder(x);
		
		auto xAxisIndex = ChunkSettings->GetVoxelIndex(x, 0, 0);
		auto yAxisIndex = ChunkSettings->GetVoxelIndex(0, x, 0);
		auto zAxisIndex = ChunkSettings->GetVoxelIndex(0, 0, x);
		
		for(int z = 0; z < ChunkLenght; z++)
		{
			for(int y = 0; y < ChunkLenght; y++)
			{
				GenerateFacesInAxis(x, y, z, xAxisIndex, minBorder, maxBorder, FrontFaceTemplate, BackFaceTemplate, faces[FRONT_FACE_INDEX], faces[BACK_FACE_INDEX]);
				GenerateFacesInAxis(y, x, z, yAxisIndex, minBorder, maxBorder, RightFaceTemplate, LeftFaceTemplate, faces[RIGHT_FACE_INDEX], faces[LEFT_FACE_INDEX]);
				GenerateFacesInAxis(z, y, x, zAxisIndex, minBorder, maxBorder, BottomFaceTemplate, TopFaceTemplate, faces[BOTTOM_FACE_INDEX], faces[TOP_FACE_INDEX]);
			}
		}
	}
}

void UDefaultChunk::GenerateFacesInAxis(int x, int y, int z, int32 axisVoxelIndex, bool isMinBorder, bool isMaxBorder,
                                        const FNaiveMeshingData& faceTemplate, const FNaiveMeshingData& reversedFaceTemplate,
                                        const TArray<TSharedPtr<TArray<FChunkFace>>>& faceContainer,
                                        const TArray<TSharedPtr<TArray<FChunkFace>>>& reversedFaceContainer)
{
	auto index = ChunkSettings->GetVoxelIndex(x, y, z);
	auto voxel = Voxels[index];
				
	if(!voxel.IsEmptyVoxel())
	{
#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Face and Reverse face generation")
	#endif
		
		auto position = FIntVector(x, y, z);
		CreateFace(faceTemplate, isMinBorder, index, position, voxel, axisVoxelIndex, faceContainer);
		CreateFace(reversedFaceTemplate, isMaxBorder, index, position, voxel, axisVoxelIndex, reversedFaceContainer);
	}
}

void UDefaultChunk::CreateFace(const FNaiveMeshingData& faceTemplate, bool isBorder,
	const int32& index, const FIntVector& position, const FVoxel& voxel, const int32& axisVoxelIndex,
	const TArray<TSharedPtr<TArray<FChunkFace>>>& faceContainer)
{

	auto chunkFaces =  faceContainer[voxelIdsInMesh[voxel.VoxelId]];

	VoxelIndexParams voxelIndexParams =
	{
		isBorder,
		faceTemplate.ForwardVoxelIndex + index,
		faceTemplate.PreviousVoxelIndex + index,
		index - axisVoxelIndex + faceTemplate.ChunkBorderIndex,
		faceTemplate.StaticMeshingData.borderChunkDirection
	};

	if(IsBorderVoxelVisible(voxelIndexParams) || IsVoxelVisible(voxelIndexParams))
	{
		FChunkFace newFace =
		{
			voxel,
			faceTemplate.StaticMeshingData.faceTemplate.StartVertexDown + position,
			faceTemplate.StaticMeshingData.faceTemplate.EndVertexDown + position,
			faceTemplate.StaticMeshingData.faceTemplate.EndVertexUp + position,
			faceTemplate.StaticMeshingData.faceTemplate.StartVertexUp + position,
		};

		if(!chunkFaces->IsEmpty())
		{
			FChunkFace& prevFace = chunkFaces->Last();
			if(faceTemplate.StaticMeshingData.naiveFaceMerge(prevFace,newFace)){
				//return when new face was merged
				return;
			}
		}

		chunkFaces->Push(newFace);
	}
}

bool UDefaultChunk::IsBorderVoxelVisible(const VoxelIndexParams& faceData) const
{
	if(faceData.isBorder)
	{
		FIntVector neighborChunkCoords = ChunkGridPos + faceData.borderChunkDirection;
		auto chunk = ChunkGridData->GetChunkPtr(neighborChunkCoords);
		return chunk != nullptr && (VisibleChunkBorders && !chunk->HasMesh() || chunk->VoxelAt(faceData.currentVoxelIndex).IsEmptyVoxel());
	}
	return false;
}

bool UDefaultChunk::IsVoxelVisible(const VoxelIndexParams& faceData)
{
	return !faceData.isBorder && Voxels.IsValidIndex(faceData.forwardVoxelIndex) && Voxels[faceData.forwardVoxelIndex].IsEmptyVoxel();
}


void UDefaultChunk::GreedyMeshAllFaces(TArray<TSharedPtr<TArray<FChunkFace>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Greedy mesh generation")
#endif
	for (auto voxelId : voxelIdsInMesh)
	{
		// Front
		GreedyMeshing(faces[FRONT_FACE_INDEX][voxelId.Value], FChunkFace::MergeFaceDown);
		
		// Back
		GreedyMeshing(faces[BACK_FACE_INDEX][voxelId.Value], FChunkFace::MergeFaceDown);
	
		// Right
		GreedyMeshing(faces[RIGHT_FACE_INDEX][voxelId.Value],FChunkFace::MergeFaceDown);

		// Left
		GreedyMeshing(faces[LEFT_FACE_INDEX][voxelId.Value],FChunkFace::MergeFaceDown);

		// Bottom
		GreedyMeshing(faces[BOTTOM_FACE_INDEX][voxelId.Value], FChunkFace::MergeFaceDown);

		// Top
		GreedyMeshing(faces[TOP_FACE_INDEX][voxelId.Value],FChunkFace::MergeFaceDown);
	}
}

void UDefaultChunk::GreedyMeshing(TSharedPtr<TArray<FChunkFace>>& faces,
                                  const TFunctionRef<bool(FChunkFace& prevFace, const FChunkFace& newFace)>& mergeFaces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Face greedy mesh generation")
#endif

	auto faceArraySize = faces->Num();

	if(faceArraySize <= 1)
	{
		return;
	}
	
	auto meshedFaces = MakeShared<TArray<FChunkFace>>();
	meshedFaces->Reserve(faceArraySize);
		
	for (int i = 1; i < faceArraySize; i++)
	{
		FChunkFace& prevFace = (*faces)[i-1];
		FChunkFace& nextFace = (*faces)[i];

		if(!mergeFaces(nextFace, prevFace))
		{
			meshedFaces->Push(prevFace);
		}
	}

	meshedFaces->Push(faces->Last());

	faces = meshedFaces;
}


void UDefaultChunk::GenerateMeshFromFaces(const TArray<TSharedPtr<TArray<FChunkFace>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh section generation")
#endif
	
		TSharedPtr<FRealtimeMeshStreamSet> StreamSet = MakeShared<FRealtimeMeshStreamSet>();

	TRealtimeMeshBuilderLocal<int32> Builder(*StreamSet.ToWeakPtr().Pin());

	Builder.EnableTexCoords();
	Builder.EnableColors();
	Builder.EnableTangents();
	
	Builder.EnablePolyGroups();

	auto voxelSize = ChunkSettings->GetVoxelSize();
	FVector3f normals[6];
	normals[FRONT_FACE_INDEX] = FVector3f(-1.0f, 0.0f, 0.0f);
	normals[BACK_FACE_INDEX] = FVector3f(1.0f, 0.0f, 0.0f);
	normals[RIGHT_FACE_INDEX] = FVector3f(0.0f, -1.0f, 0.0f);
	normals[LEFT_FACE_INDEX] = FVector3f(0.0f, 1.0f, 0.0f);
	normals[BOTTOM_FACE_INDEX] = FVector3f(0.0f, 0.0f, -1.0f);
	normals[TOP_FACE_INDEX] = FVector3f(0.0f, 0.0f, 1.0f);
	
	// Because of RealTimeMesh component voxelId needs to be first
	for (auto voxelId : voxelIdsInMesh)
	{
		for(int faceIndex = 0; faceIndex < FACE_COUNT; faceIndex++)
		{
			auto faceContainer = faces[faceIndex];

			auto voxelIndex = voxelId.Value;
			auto sideFaces = faceContainer[voxelIndex];
	
			for (auto Face : *sideFaces)
			{
				// Add our first vertex
				int32 V0 = Builder.AddVertex(Face.GetFinalStartVertexDown(voxelSize))
					.SetColor(FColor::White)
					.SetTexCoord(FVector2f(0, 0))
					.SetNormal(normals[faceIndex]);

				// Add our second vertex
				int32 V1 = Builder.AddVertex(Face.GetFinalEndVertexDown(voxelSize))
					.SetColor(FColor::White)
					.SetTexCoord(FVector2f(1, 0))
					.SetNormal(normals[faceIndex]);

				// Add our third vertex
				int32 V2 = Builder.AddVertex(Face.GetFinalEndVertexUp(voxelSize))
					.SetColor(FColor::White)
					.SetTexCoord(FVector2f(1, 1))
					.SetNormal(normals[faceIndex]);
				
				int32 V3 = Builder.AddVertex(Face.GetFinalStartVertexUp(voxelSize))
				.SetColor(FColor::White)
				.SetTexCoord(FVector2f(0, 1))
					.SetNormal(normals[faceIndex]);
				
				Builder.AddTriangle(V0, V1, V2, voxelIndex);
				Builder.AddTriangle(V2, V3, V0, voxelIndex);
			}
		}
	}
	
	AsyncTask(ENamedThreads::GameThread, [this, StreamSet]()
	{
#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Procedural mesh generation")
#endif
		
		URealtimeMeshSimple* RealtimeMesh = ChunkActor->GetRealtimeMeshComponent()->InitializeRealtimeMesh<URealtimeMeshSimple>();
			
		const FRealtimeMeshSectionGroupKey GroupKey = FRealtimeMeshSectionGroupKey::Create(0, FName("Chunk Mesh"));

		// Now we create the section group, since the stream set has polygroups, this will create the sections as well
		RealtimeMesh->CreateSectionGroup(GroupKey, *StreamSet);

		for (auto voxelId : voxelIdsInMesh)
		{
			auto voxelIndex = voxelId.Value;
			const FRealtimeMeshSectionKey PolyGroup0SectionKey = FRealtimeMeshSectionKey::CreateForPolyGroup(GroupKey, voxelIndex);
			RealtimeMesh->UpdateSectionConfig(PolyGroup0SectionKey, FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, voxelIndex), true);

			FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(voxelId.Key);
			RealtimeMesh->SetupMaterialSlot(voxelIndex, voxelType.BlockName, voxelType.Material);
		}
	});
}

bool UDefaultChunk::IsMinBorder(const int x)
{
	return x == 0;
}

bool UDefaultChunk::IsMaxBorder(const int x) const
{
	return x == ChunkLenght - 1;
}

void UDefaultChunk::GenerateVoxels()
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif
	
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	auto maxElevation = ChunkSettings->GetMaximumElevation();

	auto gridPos = ChunkGridPos * chunkLenght;
	int voxelIds = ChunkGridData->GetVoxelIdCount();
	voxelIdsInMesh.Reserve(voxelIds);
	
	for(int voxelId = 0; voxelId < voxelIds; voxelId++)
	{
		FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(voxelId);
		
		auto voxel = FVoxel(voxelId);
		
		Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, voxelType.Seed,  voxelType.NoiseFrequency);
		
		for (int x = 0; x < chunkLenght; x++) 
		{
			for (int y = 0; y < chunkLenght; y++) 
			{
				auto elevation = Noise->GetNoise2D(x + gridPos.X,  y + gridPos.Y) * maxElevation;
				
				for (int z = 0; z < chunkLenght; z++) 
				{
					auto index = ChunkSettings->GetVoxelIndex(x, y, z);
					
					if (gridPos.Z + z <= elevation) 
					{
						Voxels[index] = voxel;
						if(!voxelIdsInMesh.Contains(voxel.VoxelId))
						{
							voxelIdsInMesh.Add(voxel.VoxelId, voxelIdsInMesh.Num());
						}
						Empty = false;
					}
				}
			}
		}
	}
}

FVoxel UDefaultChunk::VoxelAt(int32 index)
{
	return Voxels[index];
}

double UDefaultChunk::GetHighestElevationAtPosition(double posX, double posY)
{
	double maxElevation = 0.0;

	auto voxelSize = ChunkSettings->GetVoxelSize();
	
	int voxelIds = ChunkGridData->GetVoxelIdCount();

	auto voxelPosX = posX / voxelSize;
	auto voxelPosY = posY / voxelSize;
	
	for(int voxelId = 0; voxelId < voxelIds; voxelId++)
	{
		FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(voxelId);
		Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, voxelType.Seed,  voxelType.NoiseFrequency);
		double elevation = Noise->GetNoise2D(voxelPosX, voxelPosY) * ChunkSettings->GetMaximumElevation();

		if(elevation > maxElevation)
		{
			maxElevation = elevation;
		}
	}
	
	return maxElevation * voxelSize;
}