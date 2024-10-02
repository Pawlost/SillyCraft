// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunks/DefaultChunk.h"

#include "FastNoiseWrapper.h"
#include "Chunks/ChunkActor.h"
#include "Chunks/ChunkGridData.h"
#include "Chunks/ChunkSettings.h"
#include "Voxels/FrontFace.h"
#include "Voxels/Voxel.h"
#include "Voxels/VoxelType.h"

void UDefaultChunk::AddToGrid(const TWeakObjectPtr<UChunkGridData> chunkGridData, FIntVector& chunkGridPos)
{
	ChunkSettings = chunkGridData->GetChunkSettings();
	Voxels.SetNum(ChunkSettings->GetChunkSideSizeInVoxels() * ChunkSettings->GetChunkPlaneSizeInVoxels());
	chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	voxelSize = ChunkSettings->GetVoxelSize();
	
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
	
	TUniquePtr<TMap<int32, TSharedPtr<TArray<UChunkFace>>>> faces[FACE_COUNT];

	InitFaces(faces);
	FaceGeneration(faces);
	GreedyMeshFaces(faces);
	GenerateMeshFromFaces(faces);
}

void UDefaultChunk::InitFaces(TUniquePtr<TMap<int32, TSharedPtr<TArray<UChunkFace>>>>* faces)
{
	for(int i = 0; i < FACE_COUNT; i++)
	{
		faces[i] = MakeUnique<TMap<int32, TSharedPtr<TArray<UChunkFace>>>>();
		
		for (auto voxelId : voxelIdsInMesh)
		{
			if(!faces[i]->Contains(voxelId))
			{
				faces[i]->Add(voxelId, MakeShared<TArray<UChunkFace>>());
			}
		}
	}
}

void UDefaultChunk::FaceGeneration(const TUniquePtr<TMap<int32, TSharedPtr<TArray<UChunkFace>>>>* faces)
{
	// culling and naive greedy meshing
	for(int x = 0; x < chunkLenght; x++)
	{
		for(int z = 0; z < chunkLenght; z++)
		{
			for(int y = 0; y < chunkLenght; y++)
			{
#if CPUPROFILERTRACE_ENABLED
				TRACE_CPUPROFILER_EVENT_SCOPE("Mesh faces generation and culling")
#endif
				GenerateFacesInXAxis(x, y, z, faces);
				GenerateFacesInYAxis(x, y, z, faces);
				GenerateFacesInZAxis(x, y, z, faces);
			}
		}
	}
}


void UDefaultChunk::GenerateFacesInXAxis(int x, int y, int z,
                                         const TUniquePtr<TMap<int32, TSharedPtr<TArray<UChunkFace>>>>* faces)
{
	auto index = ChunkSettings->GetVoxelIndex(x, y, z);
	auto voxel = Voxels[index];
				
	if(!voxel.IsEmptyVoxel())
	{
		auto position = FIntVector(x, y, z);
					
		// Front
		if(CrossChunkCullInNegativeDirection(x, index + ChunkSettings->GetVoxelIndex(-1,0,0),
			ChunkSettings->GetVoxelIndex(chunkLenght - 1, y, z), FIntVector(-1, 0, 0)))
		{
			auto face = FFrontFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[0], index + ChunkSettings->GetVoxelIndex(0,-1,0), UChunkFace::EMergeMethod::End,  UChunkFace::EUnstableAxis::Y);
		}
							
		// Back
		if(CrossChunkCullInPositiveDirection(x, index + ChunkSettings->GetVoxelIndex(1,0,0),
			ChunkSettings->GetVoxelIndex(0, y, z), FIntVector(1, 0, 0)))
		{
			auto face = UChunkFace::CreateBackFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[1], index + ChunkSettings->GetVoxelIndex(0,-1,0), UChunkFace::EMergeMethod::Begin,  UChunkFace::EUnstableAxis::Y);
		}
	}
}

void UDefaultChunk::GenerateFacesInYAxis(int x, int y, int z,
                                         const TUniquePtr<TMap<int32, TSharedPtr<TArray<UChunkFace>>>>* faces)
{
	auto index = ChunkSettings->GetVoxelIndex(z, y, x);
	auto voxel = Voxels[index];
				
	if(!voxel.IsEmptyVoxel())
	{
		auto position = FIntVector(z, y, x);
		// Bottom
		if(CrossChunkCullInNegativeDirection(x, index + ChunkSettings->GetVoxelIndex(0,0,-1),
			ChunkSettings->GetVoxelIndex(z, y, chunkLenght - 1), FIntVector(0, 0, -1)))
		{
			auto face = UChunkFace::CreateBottomFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[4], index + ChunkSettings->GetVoxelIndex(0,-1,0), UChunkFace::EMergeMethod::Begin,  UChunkFace::EUnstableAxis::Y);
		}
			
		// Top
		if(CrossChunkCullInPositiveDirection(x, index + ChunkSettings->GetVoxelIndex(0,0,1),
			ChunkSettings->GetVoxelIndex(z, y,0), FIntVector(0, 0, 1)))
		{
			auto face = UChunkFace::CreateTopFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[5], index + ChunkSettings->GetVoxelIndex(0,-1,0), UChunkFace::EMergeMethod::End,  UChunkFace::EUnstableAxis::Y);
		}
	}
}

void UDefaultChunk::GenerateFacesInZAxis(int x, int y, int z,
                                         const TUniquePtr<TMap<int32, TSharedPtr<TArray<UChunkFace>>>>* faces)
{
	auto index = ChunkSettings->GetVoxelIndex(y, x, z);
	auto voxel = Voxels[index];
				
	if(!voxel.IsEmptyVoxel())
	{
		auto position = FIntVector(y, x, z);

		// Right
		if(CrossChunkCullInNegativeDirection(x, index + ChunkSettings->GetVoxelIndex(0,-1,0),
			ChunkSettings->GetVoxelIndex(y, chunkLenght-1, z), FIntVector(0, -1, 0)))
		{
			auto face = UChunkFace::CreateRightFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[3], index + ChunkSettings->GetVoxelIndex(-1,0,0), UChunkFace::EMergeMethod::Begin, UChunkFace::EUnstableAxis::X);
		}
					
		// Left
		if(CrossChunkCullInPositiveDirection(x, index + ChunkSettings->GetVoxelIndex(0,1,0),
			ChunkSettings->GetVoxelIndex(y,0, z), FIntVector(0, 1, 0)))
		{
			auto face = UChunkFace::CreateLeftFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[2], index + ChunkSettings->GetVoxelIndex(-1,0,0), UChunkFace::EMergeMethod::End,  UChunkFace::EUnstableAxis::X);
		}
	}
}


void UDefaultChunk::AddNaiveMeshedFace(const UChunkFace& face,
                                       TMap<int32, TSharedPtr<TArray<UChunkFace>>>& faces, int32 previousVoxelDirection,
                                       UChunkFace::EMergeMethod mergeMethod, UChunkFace::EUnstableAxis unstableAxis)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Naive greedy mesh generation")
#endif
	
	auto chunkFace = faces.Find(face.GetVoxelId())->ToSharedRef();

	if(!chunkFace->IsEmpty() && chunkFace->Last().NaiveFaceMerge(face))
	{
		return;
	}

	chunkFace->Add(face);
}

bool UDefaultChunk::CrossChunkCullInNegativeDirection(int min, int32 forwardVoxelIndex, int32 chunkIndex, FIntVector neighborChunkDistance)
{
	// min == 0 is first face voxel in chunk, needs to be always shown or cross chunk compared
	if(min == 0)
	{
		return ChunkCull(chunkIndex, neighborChunkDistance);
	}
	
	return VoxelCull(forwardVoxelIndex);
}

bool UDefaultChunk::CrossChunkCullInPositiveDirection(int max, int32 forwardVoxelIndex, int32 chunkIndex,
	FIntVector neighborChunkDistance)
{
	auto chunkSize = ChunkSettings->GetChunkSideSizeInVoxels();

	if(max == chunkSize - 1)
	{
		return ChunkCull(chunkIndex, neighborChunkDistance);
	}

	// chunkSize == 0 is last face voxel in chunk, needs to be always shown or cross chunk compared
	return VoxelCull(forwardVoxelIndex);
}

bool UDefaultChunk::ChunkCull(int32 chunkIndex, const FIntVector& neighborChunkDistance) const
{
	FIntVector neighborChunkCoords = ChunkGridPos + neighborChunkDistance;
	auto chunk = ChunkGridData->GetChunkPtr(neighborChunkCoords);
	return chunk != nullptr && (VisibleChunkBorders && !chunk->HasMesh() || chunk->VoxelAt(chunkIndex).IsEmptyVoxel());
}

bool UDefaultChunk::VoxelCull(int32 forwardVoxelIndex)
{
	return Voxels.IsValidIndex(forwardVoxelIndex) && Voxels[forwardVoxelIndex].IsEmptyVoxel();
}

void UDefaultChunk::GreedyMeshFaces(const TUniquePtr<TMap<int32, TSharedPtr<TArray<UChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Greedy mesh generation")
#endif
	
	//Front
	GreedyMeshing(*faces[0], UChunkFace::EMergeMethod::Down, UChunkFace::EUnstableAxis::Z);

	//Back
	GreedyMeshing(*faces[1], UChunkFace::EMergeMethod::Down, UChunkFace::EUnstableAxis::Z);
	
	//Left
	GreedyMeshing(*faces[2], UChunkFace::EMergeMethod::Down, UChunkFace::EUnstableAxis::Z);

	//Right
	GreedyMeshing(*faces[3], UChunkFace::EMergeMethod::Down, UChunkFace::EUnstableAxis::Z);

	// Bottom
	GreedyMeshing(*faces[4], UChunkFace::EMergeMethod::Down, UChunkFace::EUnstableAxis::X);

	// Top
	GreedyMeshing(*faces[5], UChunkFace::EMergeMethod::Down, UChunkFace::EUnstableAxis::X);
}

void UDefaultChunk::GreedyMeshing(TMap<int32, TSharedPtr<TArray<UChunkFace>>>& faces,
	UChunkFace::EMergeMethod mergeMethod, UChunkFace::EUnstableAxis unstableAxis)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Face Greedy mesh generation")
#endif
	
	TArray<int32> voxelIds;
	faces.GetKeys(voxelIds);

	for (auto voxelId : voxelIds)
	{
		auto voxelFaces = faces.Find(voxelId)->ToSharedRef();
		auto voxelFacesSize = voxelFaces->Num();

		if(voxelFacesSize <= 1)
		{
			continue;
		}
		
		TSharedPtr<TArray<UChunkFace>> meshedFaces = MakeShared<TArray<UChunkFace>>();
		
		for (int i = 1; i < voxelFacesSize; i++)
		{
			UChunkFace& prevFace = (*voxelFaces)[i-1];
			UChunkFace& nextFace = (*voxelFaces)[i];

			if(!nextFace.GreedyFaceMerge(prevFace))
			{
				meshedFaces->Add(prevFace);
			}
		}

		meshedFaces->Add((*voxelFaces)[voxelFacesSize-1]);

		faces.Remove(voxelId);
		faces.Add(voxelId, meshedFaces);
	}
}

void UDefaultChunk::GenerateMeshFromFaces(const TUniquePtr<TMap<int32, TSharedPtr<TArray<UChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation")
#endif
		
	for (auto voxelId : voxelIdsInMesh)
	{

#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Mesh section generation")
	#endif
		
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
				Vertice->Push(Face.GetStartVertexDown(voxelSize));
				Vertice->Push(Face.GetEndVertexDown(voxelSize));
				Vertice->Push(Face.GetEndVertexUp(voxelSize));
				Vertice->Push(Face.GetStartVertexUp(voxelSize));

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