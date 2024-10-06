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
	Voxels.SetNum(ChunkSettings->GetChunkSideSizeInVoxels() * ChunkSettings->GetChunkPlaneSizeInVoxels());
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
	
	TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>> faces[FACE_COUNT];

	InitFaceContainers(faces);
	FaceGeneration(faces);
	GreedyMeshAllFaces(faces);
	GenerateMeshFromFaces(faces);
}

void UDefaultChunk::InitFaceContainers(TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
	for(int i = 0; i < FACE_COUNT; i++)
	{
		faces[i] = MakeUnique<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>();
		
		for (auto voxelId : voxelIdsInMesh)
		{
			if(!faces[i]->Contains(voxelId))
			{
				faces[i]->Add(voxelId, MakeShared<TArray<FChunkFace>>());
			}
		}
	}
}

void UDefaultChunk::FaceGeneration(const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Naive greedy mesh generation")
#endif
	
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	
	// culling and naive greedy meshing
	for(int x = 0; x < chunkLenght; x++)
	{
		for(int z = 0; z < chunkLenght; z++)
		{
			for(int y = 0; y < chunkLenght; y++)
			{

#if CPUPROFILERTRACE_ENABLED
				TRACE_CPUPROFILER_EVENT_SCOPE("Face generation")
			#endif
				
				auto minBorder = x == 0;
				auto maxBorder = x == chunkLenght - 1;

					
				NaiveMeshingData frontFaceData =
				{
					minBorder,
					ChunkSettings->GetVoxelIndex(-1,0,0),
					ChunkSettings->GetVoxelIndex(chunkLenght - 1, y, z),
					FIntVector(-1, 0, 0),
					FChunkFace::FrontFace,
					ChunkSettings->GetVoxelIndex(0,-1,0),
					FChunkFace::EMergeMethod::End,
					FChunkFace::EUnstableAxis::Y
				};
				
				NaiveMeshingData backFaceData =
				{
					maxBorder,
					ChunkSettings->GetVoxelIndex(1,0,0),
					ChunkSettings->GetVoxelIndex(0, y, z),
					FIntVector(1, 0, 0),
					FChunkFace::BackFace,
					ChunkSettings->GetVoxelIndex(0,-1,0),
					FChunkFace::EMergeMethod::Begin,
					FChunkFace::EUnstableAxis::Y
				};


				NaiveMeshingData rightFaceData =
				{
					minBorder,
					ChunkSettings->GetVoxelIndex(0,-1,0),
					ChunkSettings->GetVoxelIndex(y, chunkLenght-1, z),
					FIntVector(0, -1, 0),
					FChunkFace::RightFace,
					ChunkSettings->GetVoxelIndex(-1,0,0),
					FChunkFace::EMergeMethod::Begin,
					FChunkFace::EUnstableAxis::X
				};
				
				NaiveMeshingData leftFaceData =
				{
					maxBorder,
					ChunkSettings->GetVoxelIndex(0,1,0),
					ChunkSettings->GetVoxelIndex(y, 0, z),
					FIntVector(0, 1, 0),
					FChunkFace::LeftFace,
					ChunkSettings->GetVoxelIndex(-1,0,0),
					FChunkFace::EMergeMethod::End,
					FChunkFace::EUnstableAxis::X
				};


				NaiveMeshingData bottomFaceData =
				{
					minBorder,
					ChunkSettings->GetVoxelIndex(0,0,-1),
					ChunkSettings->GetVoxelIndex(z, y, chunkLenght - 1),
					FIntVector(0, 0, -1),
					FChunkFace::BottomFace,
					ChunkSettings->GetVoxelIndex(0,-1,0),
					FChunkFace::EMergeMethod::Begin,
					FChunkFace::EUnstableAxis::Y
				};
				
				NaiveMeshingData topFaceData =
				{
					maxBorder,
					ChunkSettings->GetVoxelIndex(0,0,1),
					ChunkSettings->GetVoxelIndex(z, y, 0),
					FIntVector(0, 0, 1),
					FChunkFace::TopFace,
					ChunkSettings->GetVoxelIndex(0,-1,0),
					FChunkFace::EMergeMethod::End,
					FChunkFace::EUnstableAxis::Y
				};
				
				GenerateFacesInAxis(x, y, z,  frontFaceData, backFaceData, *faces[0], *faces[1]);
				GenerateFacesInAxis(y, x, z, rightFaceData, leftFaceData, *faces[2], *faces[3]);
				GenerateFacesInAxis(z, y, x,  bottomFaceData, topFaceData, *faces[4], *faces[5]);
			}
		}
	}
}

void UDefaultChunk::GenerateFacesInAxis(int x, int y, int z,
										NaiveMeshingData& faceData, NaiveMeshingData& reversedFaceData,
                                        const TMap<int32, TSharedPtr<TArray<FChunkFace>>>& faceContainer,
                                        const TMap<int32, TSharedPtr<TArray<FChunkFace>>>& reversedFaceContainer)
{
	auto index = ChunkSettings->GetVoxelIndex(x, y, z);
	auto voxel = Voxels[index];
				
	if(!voxel.IsEmptyVoxel())
	{
		auto position = FIntVector(x, y, z);
		CreateFace(faceData, index, position, voxel, faceContainer);
		CreateFace(reversedFaceData, index, position, voxel, reversedFaceContainer);
	}
} 

void UDefaultChunk::CreateFace(NaiveMeshingData& faceData, const int32& index, const FIntVector& position, const FVoxel& voxel, const TMap<int32, TSharedPtr<TArray<FChunkFace>>>& faceContainer)
{
	auto chunkFaces =  *faceContainer.Find(voxel.VoxelId);
	faceData.forwardVoxelIndex += index;
	faceData.previousVoxelIndex += index;
	faceData.face.Voxel = voxel;
	faceData.face.StartVertexDown += position;
	faceData.face.EndVertexDown += position;
	faceData.face.StartVertexUp += position;
	faceData.face.EndVertexUp += position;

	/*	if((ChunkCull(faceData) || VoxelCull(faceData)) &&
			!NaiveMeshing(faceData, *chunkFaces))
		{
			chunkFaces->Add(faceData.face);
		}*/
	
	if(ChunkCull(faceData) && VoxelCull(faceData) && !NaiveMeshing(faceData, *chunkFaces))
	{
		return;
	}
	
	chunkFaces->Add(faceData.face);
}

bool UDefaultChunk::ChunkCull(const NaiveMeshingData& faceData) const
{
	if(!faceData.isBorder)
	{
		return false;
	}
	
	FIntVector neighborChunkCoords = ChunkGridPos + faceData.neighborChunkDistance;
	auto chunk = ChunkGridData->GetChunkPtr(neighborChunkCoords);
	return chunk != nullptr && (VisibleChunkBorders && !chunk->HasMesh() || chunk->VoxelAt(faceData.neighborChunkVoxelIndex).IsEmptyVoxel());
}

bool UDefaultChunk::VoxelCull(const NaiveMeshingData& faceData)
{
	return !faceData.isBorder && Voxels.IsValidIndex(faceData.forwardVoxelIndex) && Voxels[faceData.forwardVoxelIndex].IsEmptyVoxel();
}

bool UDefaultChunk::NaiveMeshing(const NaiveMeshingData& naiveMeshingData, TArray<FChunkFace>& chunkFaces)
{
	return Voxels.IsValidIndex(naiveMeshingData.previousVoxelIndex) &&
		naiveMeshingData.face.Voxel == Voxels[naiveMeshingData.previousVoxelIndex] &&
		!chunkFaces.IsEmpty() &&
		chunkFaces.Last().MergeFace(naiveMeshingData.face, naiveMeshingData.mergeMethod, naiveMeshingData.unstableAxis);
}

void UDefaultChunk::GreedyMeshAllFaces(const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Greedy mesh generation")
#endif
	
	for (auto voxelId : voxelIdsInMesh)
	{
		// Front
		GreedyMeshing(voxelId,*faces[0], FChunkFace::EMergeMethod::Down, FChunkFace::EUnstableAxis::Z);
		
		// Back
		GreedyMeshing(voxelId,*faces[1], FChunkFace::EMergeMethod::Down, FChunkFace::EUnstableAxis::Z);
	
		// Right
		GreedyMeshing(voxelId,*faces[2], FChunkFace::EMergeMethod::Down, FChunkFace::EUnstableAxis::Z);

		// Left
		GreedyMeshing(voxelId,*faces[3], FChunkFace::EMergeMethod::Down, FChunkFace::EUnstableAxis::Z);

		// Bottom
		GreedyMeshing(voxelId,*faces[4], FChunkFace::EMergeMethod::Down, FChunkFace::EUnstableAxis::X);

		// Top
		GreedyMeshing(voxelId,*faces[5], FChunkFace::EMergeMethod::Down, FChunkFace::EUnstableAxis::X);
	}
}

void UDefaultChunk::GreedyMeshing(int32 voxelId, TMap<int32, TSharedPtr<TArray<FChunkFace>>>& faces,
								  FChunkFace::EMergeMethod mergeMethod, FChunkFace::EUnstableAxis unstableAxis)
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
		
	for (int i = 1; i < faceArraySize; i++)
	{
		FChunkFace& prevFace = (*voxelFacesArray)[i-1];
		FChunkFace& nextFace = (*voxelFacesArray)[i];

		if(!nextFace.MergeFace(prevFace,mergeMethod,unstableAxis))
		{
			meshedFaces->Add(prevFace);
		}
	}

	meshedFaces->Add(voxelFacesArray->Last());

	faces[voxelId] = meshedFaces;
}

void UDefaultChunk::GenerateMeshFromFaces(const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh section generation")
#endif
	
	for (auto voxelId : voxelIdsInMesh)
	{
		int32 faceIndex = 0;
	
		TSharedPtr<TArray<FVector>> Vertice = MakeShared<TArray<FVector>>();
		TSharedPtr<TArray<FVector2D>> UVs = MakeShared<TArray<FVector2D>>();
		TSharedPtr<TArray<int32>> Triangles = MakeShared<TArray<int32>>();
		
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

				UVs->Add(FVector2D(0, 0));
				UVs->Add(FVector2D(1, 0));
				UVs->Add(FVector2D(1, 1));
				UVs->Add(FVector2D(0, 1));
				
				Triangles->Add(faceIndex); Triangles->Add(faceIndex + 1); Triangles->Add(faceIndex + 2);
				Triangles->Add(faceIndex + 2); Triangles->Add(faceIndex + 3); Triangles->Add(faceIndex);

				faceIndex += 4;
			}
		}

		AsyncTask(ENamedThreads::GameThread, [this, voxelId, Vertice, Triangles, UVs, voxelType]()
		{
#if CPUPROFILERTRACE_ENABLED
			TRACE_CPUPROFILER_EVENT_SCOPE("Procedural mesh generation")
#endif
			auto procMesh = ChunkActor->GetProceduralMeshComponent();

		 if(procMesh.IsValid() && procMesh.IsValid(false,true) &&  Vertice.IsValid() && Triangles.IsValid()){
			 procMesh->CreateMeshSection_LinearColor(voxelId,*Vertice, *Triangles, TArray<FVector>(), *UVs, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
			 procMesh->SetMaterial(voxelId, voxelType.Material);
			 procMesh->SetMeshSectionVisible(voxelId, true);
		 }});
	}
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