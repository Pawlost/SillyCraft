// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunks/DefaultChunk.h"

#include "FastNoiseWrapper.h"
#include "Chunks/ChunkActor.h"
#include "Chunks/ChunkGridData.h"
#include "Chunks/ChunkSettings.h"
#include "Voxels/Voxel.h"
#include "Voxels/VoxelType.h"

void UDefaultChunk::AddToGrid(const TWeakObjectPtr<UChunkGridData> chunkGridData, FIntVector& chunkGridPos)
{
	ChunkSettings = chunkGridData->GetChunkSettings();
	ChunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	Voxels.SetNum(ChunkLenght * ChunkSettings->GetChunkPlaneSizeInVoxels());
	Super::AddToGrid(chunkGridData, chunkGridPos);
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
	
	TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>> faces[FACE_COUNT];

	InitFaceContainers(faces);
	FaceGeneration(faces);
	GreedyMeshAllFaces(faces);
	GenerateMeshFromFaces(faces);
}

void UDefaultChunk::InitFaceContainers(TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
	int32 chunkPlane = ChunkSettings->GetChunkPlaneSizeInVoxels();
	for(int i = 0; i < FACE_COUNT; i++)
	{
		faces[i] = MakeShared<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>();
		
		for (auto voxelId : voxelIdsInMesh)
		{
			if(!faces[i]->Contains(voxelId))
			{
				auto faceArray = MakeShared<TArray<FChunkFace>>();
				faceArray->Reserve(chunkPlane);
				faces[i]->Add(voxelId, faceArray);
			}
		}
	}
}

void UDefaultChunk::FaceGeneration(const TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Naive greedy mesh generation")
#endif
	
	NaiveMeshingData frontFaceData =
	{
		false,
		ChunkSettings->GetVoxelIndex(-1,0,0),
		0,
		FIntVector(-1, 0, 0),
		FChunkFace::FrontFace,
		ChunkSettings->GetVoxelIndex(0,-1,0),
		FChunkFace::MergeFaceEnd
	};
				
	NaiveMeshingData backFaceData =
	{
		false,
		ChunkSettings->GetVoxelIndex(1,0,0),
		0,
		FIntVector(1, 0, 0),
		FChunkFace::BackFace,
		ChunkSettings->GetVoxelIndex(0,-1,0),
		FChunkFace::MergeFaceStart
	};

	NaiveMeshingData rightFaceData =
	{
		false,
		ChunkSettings->GetVoxelIndex(0,-1,0),
		0,
		FIntVector(0, -1, 0),
		FChunkFace::RightFace,
		ChunkSettings->GetVoxelIndex(-1,0,0),
		FChunkFace::MergeFaceStart
	};
				
	NaiveMeshingData leftFaceData =
	{
		false,
		ChunkSettings->GetVoxelIndex(0,1,0),
		0,
		FIntVector(0, 1, 0),
		FChunkFace::LeftFace,
		ChunkSettings->GetVoxelIndex(-1,0,0),
		FChunkFace::MergeFaceEnd
	};


	NaiveMeshingData bottomFaceData =
	{
		false,
		ChunkSettings->GetVoxelIndex(0,0,-1),
		0,
		FIntVector(0, 0, -1),
		FChunkFace::BottomFace,
		ChunkSettings->GetVoxelIndex(0,-1,0),
		FChunkFace::MergeFaceStart

	};
				
	NaiveMeshingData topFaceData =
	{
		false,
		ChunkSettings->GetVoxelIndex(0,0,1),
		0,
		FIntVector(0, 0, 1),
		FChunkFace::TopFace,
		ChunkSettings->GetVoxelIndex(0,-1,0),
		FChunkFace::MergeFaceEnd
	};
	
	// culling and naive greedy meshing
	for(int x = 0; x < ChunkLenght; x++)
	{
		frontFaceData.isBorder = IsMinBorder(x);
		backFaceData.isBorder = IsMaxBorder(x);
		rightFaceData.isBorder =  IsMinBorder(x);
		leftFaceData.isBorder = IsMaxBorder(x);
		bottomFaceData.isBorder = IsMinBorder(x);
		topFaceData.isBorder = IsMaxBorder(x);
		
		for(int z = 0; z < ChunkLenght; z++)
		{
			for(int y = 0; y < ChunkLenght; y++)
			{
				frontFaceData.neighborChunkVoxelIndex = ChunkSettings->GetVoxelIndex(ChunkLenght - 1, y, z);
				backFaceData.neighborChunkVoxelIndex = ChunkSettings->GetVoxelIndex(0, y, z);
				GenerateFacesInAxis(x, y, z,  frontFaceData, backFaceData, faces[0].ToSharedRef(), faces[1].ToSharedRef());

				rightFaceData.neighborChunkVoxelIndex = ChunkSettings->GetVoxelIndex(y, ChunkLenght-1, z);
				leftFaceData.neighborChunkVoxelIndex = ChunkSettings->GetVoxelIndex(y, 0, z);
				GenerateFacesInAxis(y, x, z, rightFaceData, leftFaceData, faces[2].ToSharedRef(), faces[3].ToSharedRef());
				
				bottomFaceData.neighborChunkVoxelIndex = ChunkSettings->GetVoxelIndex(z, y, ChunkLenght - 1);
				topFaceData.neighborChunkVoxelIndex = ChunkSettings->GetVoxelIndex(z, y, 0);
				GenerateFacesInAxis(z, y, x,  bottomFaceData, topFaceData, faces[4].ToSharedRef(), faces[5].ToSharedRef());
			}
		}
	}
}

void UDefaultChunk::GenerateFacesInAxis(int x, int y, int z,
                                        NaiveMeshingData faceData, NaiveMeshingData reversedFaceData,
                                        const TSharedRef<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>& faceContainer,
                                        const TSharedRef<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>& reversedFaceContainer)
{
	auto index = ChunkSettings->GetVoxelIndex(x, y, z);
	auto voxel = Voxels[index];
				
	if(!voxel.IsEmptyVoxel())
	{
#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Face generation")
	#endif
		
		auto position = FIntVector(x, y, z);
		CreateFace(faceData, index, position, voxel, faceContainer);
		CreateFace(reversedFaceData, index, position, voxel, reversedFaceContainer);
	}
}

bool UDefaultChunk::ChunkCull(const NaiveMeshingData& faceData) const
{
	if(faceData.isBorder)
	{
		FIntVector neighborChunkCoords = ChunkGridPos + faceData.neighborChunkDistance;
		auto chunk = ChunkGridData->GetChunkPtr(neighborChunkCoords);
		return chunk != nullptr && (VisibleChunkBorders && !chunk->HasMesh() || chunk->VoxelAt(faceData.neighborChunkVoxelIndex).IsEmptyVoxel());
	}
	return false;
}

bool UDefaultChunk::VoxelCull(const NaiveMeshingData& faceData)
{
	return !faceData.isBorder && Voxels.IsValidIndex(faceData.forwardVoxelIndex) && Voxels[faceData.forwardVoxelIndex].IsEmptyVoxel();
}


void UDefaultChunk::CreateFace(NaiveMeshingData& faceData, const int32& index, const FIntVector& position, const FVoxel& voxel, const TSharedRef<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>& faceContainer)
{
	auto chunkFaces =  faceContainer->Find(voxel.VoxelId)->ToSharedRef();
		
	faceData.forwardVoxelIndex += index;
	faceData.previousVoxelIndex += index;
	faceData.face.Voxel = voxel;
	faceData.face.StartVertexDown += position;
	faceData.face.EndVertexDown += position;
	faceData.face.StartVertexUp += position;
	faceData.face.EndVertexUp += position;

	if((ChunkCull(faceData) || VoxelCull(faceData)))
	{
		if(!chunkFaces->IsEmpty() && voxel == faceData.face.Voxel && faceData.mergeFaces(chunkFaces->Last(),faceData.face)){
			return;
		}

		chunkFaces->Push(faceData.face);
	}
}

void UDefaultChunk::GreedyMeshAllFaces(const TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Greedy mesh generation")
#endif
	for (auto voxelId : voxelIdsInMesh)
	{
		// Front
		GreedyMeshing(voxelId,*faces[0], FChunkFace::MergeFaceDown);
		
		// Back
		GreedyMeshing(voxelId,*faces[1], FChunkFace::MergeFaceDown);
	
		// Right
		GreedyMeshing(voxelId,*faces[2],FChunkFace::MergeFaceDown);

		// Left
		GreedyMeshing(voxelId,*faces[3],FChunkFace::MergeFaceDown);

		// Bottom
		GreedyMeshing(voxelId,*faces[4], FChunkFace::MergeFaceDown);

		// Top
		GreedyMeshing(voxelId,*faces[5],FChunkFace::MergeFaceDown);
	}
}

void UDefaultChunk::GreedyMeshing(int32 voxelId, TMap<int32, TSharedPtr<TArray<FChunkFace>>>& faces,
								  TFunctionRef<bool(FChunkFace& prevFace, const FChunkFace& newFace)> mergeFaces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Face greedy mesh generation")
#endif
	
	auto voxelFaces = faces.Find(voxelId);

	if(voxelFaces == nullptr || (*voxelFaces)->Num() <= 1)
	{
		return;
	}

	auto voxelFacesArray = voxelFaces->ToSharedRef();
	auto faceArraySize = voxelFacesArray->Num();
	
	auto meshedFaces = MakeShared<TArray<FChunkFace>>();
	meshedFaces->Reserve(faceArraySize);
		
	for (int i = 1; i < faceArraySize; i++)
	{
		FChunkFace& prevFace = (*voxelFacesArray)[i-1];
		FChunkFace& nextFace = (*voxelFacesArray)[i];

		if(!mergeFaces(nextFace, prevFace))
		{
			meshedFaces->Push(prevFace);
		}
	}

	meshedFaces->Push(voxelFacesArray->Last());

	faces[voxelId] = meshedFaces;
}


void UDefaultChunk::GenerateMeshFromFaces(const TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh section generation")
#endif
	
	for (auto voxelId : voxelIdsInMesh)
	{
		int32 faceIndex = 0;
	
		TSharedPtr<TArray<FVector>> Vertice = MakeShared<TArray<FVector>>();
		TSharedPtr<TArray<FVector2D>> UVs = MakeShared<TArray<FVector2D>>();
		TSharedPtr<TArray<int32>> Indice = MakeShared<TArray<int32>>();
		
		FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(voxelId);

		for(int i = 0; i < FACE_COUNT; i++)
		{
			auto sideFaces = faces[i]->Find(voxelId);
			
			if(sideFaces == nullptr)
			{
				continue;
			}
	
			for (auto Face : *sideFaces->Get())
			{
				auto voxelSize = ChunkSettings->GetVoxelSize();

				Vertice->Push(Face.GetFinalStartVertexDown(voxelSize));
				Vertice->Push(Face.GetFinalEndVertexDown(voxelSize));
				Vertice->Push(Face.GetFinalEndVertexUp(voxelSize));
				Vertice->Push(Face.GetFinalStartVertexUp(voxelSize));

				UVs->Emplace(0, 0);
				UVs->Emplace(1, 0);
				UVs->Emplace(1, 1);
				UVs->Emplace(0, 1);
				
				Indice->Append({faceIndex, faceIndex + 1, faceIndex + 2, faceIndex + 2, faceIndex + 3, faceIndex});
				faceIndex += 4;
			}
		}

		AsyncTask(ENamedThreads::GameThread, [this, voxelId, Vertice, Indice, UVs, voxelType]()
		{
#if CPUPROFILERTRACE_ENABLED
			TRACE_CPUPROFILER_EVENT_SCOPE("Procedural mesh generation")
#endif
			auto procMesh = ChunkActor->GetProceduralMeshComponent();

		 if(procMesh.IsValid() && procMesh.IsValid(false,true) &&  Vertice.IsValid() && Indice.IsValid()){
			 procMesh->CreateMeshSection_LinearColor(voxelId,*Vertice, *Indice, TArray<FVector>(), *UVs, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
			 procMesh->SetMaterial(voxelId, voxelType.Material);
			 procMesh->SetMeshSectionVisible(voxelId, true);
		 }});
	}
}

bool UDefaultChunk::IsMinBorder(int x)
{
	return x == 0;
}

bool UDefaultChunk::IsMaxBorder(int x) const
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
						voxelIdsInMesh.FindOrAdd(voxel.VoxelId);
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