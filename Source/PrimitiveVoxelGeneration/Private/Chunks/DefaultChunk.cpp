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

	InitFaces(faces);
	FaceGeneration(faces);
	GreedyMeshAllFaces(faces);
	GenerateMeshFromFaces(faces);
}

void UDefaultChunk::InitFaces(TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
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
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();

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
                                         const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	auto index = ChunkSettings->GetVoxelIndex(x, y, z);
	auto voxel = Voxels[index];
				
	if(!voxel.IsEmptyVoxel())
	{
		auto position = FIntVector(x, y, z);
					
		// Front
		if(CrossChunkCullInNegativeDirection(x, index + ChunkSettings->GetVoxelIndex(-1,0,0),
			ChunkSettings->GetVoxelIndex(chunkLenght - 1, y, z), FIntVector(-1, 0, 0)))
		{
			auto face = FChunkFace::CreateFrontFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[0], index + ChunkSettings->GetVoxelIndex(0,-1,0), FChunkFace::EMergeMethod::End,  FChunkFace::EUnstableAxis::Y);
		}
							
		// Back
		if(CrossChunkCullInPositiveDirection(x, index + ChunkSettings->GetVoxelIndex(1,0,0),
			ChunkSettings->GetVoxelIndex(0, y, z), FIntVector(1, 0, 0)))
		{
			auto face = FChunkFace::CreateBackFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[1], index + ChunkSettings->GetVoxelIndex(0,-1,0), FChunkFace::EMergeMethod::Begin,  FChunkFace::EUnstableAxis::Y);
		}
	}
}

void UDefaultChunk::GenerateFacesInYAxis(int x, int y, int z,
                                         const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	auto index = ChunkSettings->GetVoxelIndex(z, y, x);
	auto voxel = Voxels[index];
				
	if(!voxel.IsEmptyVoxel())
	{
		auto position = FIntVector(z, y, x);
		// Bottom
		if(CrossChunkCullInNegativeDirection(x, index + ChunkSettings->GetVoxelIndex(0,0,-1),
			ChunkSettings->GetVoxelIndex(z, y, chunkLenght - 1), FIntVector(0, 0, -1)))
		{
			auto face = FChunkFace::CreateBottomFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[4], index + ChunkSettings->GetVoxelIndex(0,-1,0), FChunkFace::EMergeMethod::Begin,  FChunkFace::EUnstableAxis::Y);
		}
			
		// Top
		if(CrossChunkCullInPositiveDirection(x, index + ChunkSettings->GetVoxelIndex(0,0,1),
			ChunkSettings->GetVoxelIndex(z, y,0), FIntVector(0, 0, 1)))
		{
			auto face = FChunkFace::CreateTopFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[5], index + ChunkSettings->GetVoxelIndex(0,-1,0), FChunkFace::EMergeMethod::End,  FChunkFace::EUnstableAxis::Y);
		}
	}
}

void UDefaultChunk::GenerateFacesInZAxis(int x, int y, int z,
                                         const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
	auto chunkLenght = ChunkSettings->GetChunkSideSizeInVoxels();
	auto index = ChunkSettings->GetVoxelIndex(y, x, z);
	auto voxel = Voxels[index];
				
	if(!voxel.IsEmptyVoxel())
	{
		auto position = FIntVector(y, x, z);

		// Right
		if(CrossChunkCullInNegativeDirection(x, index + ChunkSettings->GetVoxelIndex(0,-1,0),
			ChunkSettings->GetVoxelIndex(y, chunkLenght-1, z), FIntVector(0, -1, 0)))
		{
			auto face = FChunkFace::CreateRightFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[3], index + ChunkSettings->GetVoxelIndex(-1,0,0), FChunkFace::EMergeMethod::Begin, FChunkFace::EUnstableAxis::X);
		}
					
		// Left
		if(CrossChunkCullInPositiveDirection(x, index + ChunkSettings->GetVoxelIndex(0,1,0),
			ChunkSettings->GetVoxelIndex(y,0, z), FIntVector(0, 1, 0)))
		{
			auto face = FChunkFace::CreateLeftFace(position, voxel);
			AddNaiveMeshedFace(face, *faces[2], index + ChunkSettings->GetVoxelIndex(-1,0,0), FChunkFace::EMergeMethod::End,  FChunkFace::EUnstableAxis::X);
		}
	}
}


void UDefaultChunk::AddNaiveMeshedFace(const FChunkFace& face,
                                       TMap<int32, TSharedPtr<TArray<FChunkFace>>>& faces, int32 previousVoxelDirection,
                                       FChunkFace::EMergeMethod mergeMethod, FChunkFace::EUnstableAxis unstableAxis)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Naive greedy mesh generation")
#endif
	
	auto chunkFace = faces.Find(face.Voxel.VoxelId)->ToSharedRef();

	auto faceJoined = (Voxels.IsValidIndex(previousVoxelDirection) &&
		face.Voxel == Voxels[previousVoxelDirection] &&
		!chunkFace->IsEmpty() &&
		chunkFace->Last().MergeFace(face, mergeMethod, unstableAxis));
	
	if(faceJoined)
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

void UDefaultChunk::GreedyMeshAllFaces(const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Greedy mesh generation")
#endif
	for (auto voxelId : voxelIdsInMesh)
	{
		//Front
		GreedyMeshing(voxelId,*faces[0], FChunkFace::EMergeMethod::Down, FChunkFace::EUnstableAxis::Z);
		
		//Back
		GreedyMeshing(voxelId,*faces[1], FChunkFace::EMergeMethod::Down, FChunkFace::EUnstableAxis::Z);
	
		//Left
		GreedyMeshing(voxelId,*faces[2], FChunkFace::EMergeMethod::Down, FChunkFace::EUnstableAxis::Z);

		//Right
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
	TRACE_CPUPROFILER_EVENT_SCOPE("Face Greedy mesh generation")
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