// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/AreaChunkSpawner.h"

#include "MeshingStructs/DirectionToFace.h"

void AAreaChunkSpawner::BeginPlay()
{
	Super::BeginPlay();

/*	auto lmao1 =  GetActorLocation();
	auto lmao = ChunkMesher->GetVoxelSize();
	auto actorSnap = lmao1 / lmao;
	auto actorGridLocation = static_cast<FIntVector>(UE::Math::TVector( FMath::Floor(actorSnap.X) , FMath::Floor(actorSnap.Y) , FMath::Floor(actorSnap.Z)) );*/
	GenerateArea(FIntVector(0, 0, 0));
}

void AAreaChunkSpawner::AddChunkFromGrid(FChunkFaceParams& params, FDirectionToFace faceDirection)
{
	auto chunk = ChunkGrid.Find(params.ChunkParams.OriginalChunk->GridPosition + faceDirection.Direction);
	AddSideChunk(params, faceDirection.FaceSide, *chunk);
}

void AAreaChunkSpawner::GenerateArea(const FIntVector& gridPosition)
{

	auto minPosition = gridPosition - FIntVector(SpawnRadius);
	auto maxPosition = gridPosition + FIntVector(SpawnRadius);
	// AsyncTask(ENamedThreads::AnyThread, [this, minPosition, maxPosition]()
	// {
		for (int32 x = minPosition.X - 1; x < maxPosition.X + 1; x++)
		{
			for (int32 y = minPosition.Y - 1; y < maxPosition.Y + 1; y++)
			{
				for (int32 z = minPosition.Z - 1; z < maxPosition.Z + 1; z++)
				{
					FIntVector gridPosition(x, y, z);
					if(!ChunkGrid.Contains(gridPosition))
					{
						auto Chunk = MakeShared<FChunkStruct>().ToSharedPtr();
						SpawnChunk(Chunk, gridPosition);
					}
				}
			}
		}

		for (auto chunkLocation : ChunkGrid)
		{
			if (!chunkLocation.Value->HasMesh)
			{
				FChunkFaceParams chunkParams;
				chunkParams.ChunkParams.OriginalChunk = chunkLocation.Value.ToSharedRef();
				AddChunkFromGrid(chunkParams, FDirectionToFace::TopDirection);
				AddChunkFromGrid(chunkParams, FDirectionToFace::BottomDirection);
				AddChunkFromGrid(chunkParams, FDirectionToFace::RightDirection);
				AddChunkFromGrid(chunkParams, FDirectionToFace::LeftDirection);
				AddChunkFromGrid(chunkParams, FDirectionToFace::FrontDirection);
				AddChunkFromGrid(chunkParams, FDirectionToFace::BackDirection);
				ChunkMesher->GenerateMesh(chunkParams);
			}
		}
	// });
}
