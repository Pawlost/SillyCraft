// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/Area/CenterAreaChunkSpawner.h"

void ACenterAreaChunkSpawner::GenerateArea()
{
	auto initialCenter = CenterGridPosition;
	TSet<FIntVector> VisitedSpawnPositions;
	VisitedSpawnPositions.Reserve(SpawnZone * SpawnZone * SpawnZone * FACE_SIDE_COUNT);
	TQueue<FIntVector> SpawnPositionsArray;
	SpawnPositionsArray.Enqueue(initialCenter);
	SpawnChunk(initialCenter);
	FChunkFaceParams faceParams;
	GenerateChunkMesh(faceParams, initialCenter);

	TTuple<FGridDirectionToFace, int32> Directions[6] = {
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::FrontDirection, SpawnZone),
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::RightDirection, SpawnZone),
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::LeftDirection, SpawnZone),
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::BackDirection, SpawnZone),
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::TopDirection, ChunksAboveSpawner),
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::BottomDirection, ChunksBelowSpawner)
	};

	FIntVector centerPosition;
	VisitedSpawnPositions.Add(CenterGridPosition);

	while (IsValid(this) && SpawnPositionsArray.Dequeue(centerPosition) && initialCenter == CenterGridPosition)
	{
		for (int32 s = 0; s < FACE_SIDE_COUNT; s++)
		{
			auto direction = Directions[s];
			
			auto position = centerPosition + direction.Key.Direction;

			auto chunkPtr = ChunkGrid.Find(position);

			if (FVector::Distance(FVector(initialCenter), FVector(position)) < direction.Value + BufferZone)
			{
				if (chunkPtr == nullptr)
				{
					SpawnChunk(position);
				}

				if (!VisitedSpawnPositions.Contains(position))
				{
					VisitedSpawnPositions.Add(position);
					SpawnPositionsArray.Enqueue(position);
				}
			}
		}

		//Mesh
		if (FVector::Distance(FVector(initialCenter), FVector(centerPosition)) < SpawnZone)
		{
			GenerateChunkMesh(faceParams, centerPosition);
		}
	}
}
