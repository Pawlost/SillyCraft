// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/Area/PreloadedVoxelCenterAreaChunkSpawner.h"

void APreloadedVoxelCenterAreaChunkSpawner::GenerateArea()
{
	auto initialCenter = CenterGridPosition;
	TSet<FIntVector> VisitedSpawnPositions;
	VisitedSpawnPositions.Reserve(SpawnZone * SpawnZone * SpawnZone * FACE_SIDE_COUNT);
	TQueue<FIntVector> SpawnPositionsArray;
	SpawnChunk(initialCenter);
	FChunkFaceParams faceParams;
	TArray<TSharedFuture<void>> tasks;
	tasks.Reserve(6);
	
	SpawnPositionsArray.Enqueue(initialCenter);

	TTuple<FGridDirectionToFace, int32> Directions[6] = {
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::FrontDirection, SpawnZone),
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::RightDirection, SpawnZone),
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::LeftDirection, SpawnZone),
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::BackDirection, SpawnZone),
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::TopDirection, SpawnZone),
		TTuple<FGridDirectionToFace, int32>(FGridDirectionToFace::BottomDirection, SpawnZone)
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
					TSharedFuture<void> task;
					SpawnChunk(position, &task);

					if (task.IsValid() && !task.IsReady())
					{
						tasks.Add(task);
					}
				}

				if (!VisitedSpawnPositions.Contains(position))
				{
					VisitedSpawnPositions.Add(position);
					SpawnPositionsArray.Enqueue(position);
				}
			}
		}

		if (tasks.Num() >= 6)
		{
			WaitForAllTasks(tasks);
		}
	}

	VisitedSpawnPositions.Empty();
	SpawnPositionsArray.Enqueue(initialCenter);
	while (IsValid(this) && SpawnPositionsArray.Dequeue(centerPosition) && initialCenter == CenterGridPosition)
	{
		for (int32 s = 0; s < FACE_SIDE_COUNT; s++)
		{
			auto direction = Directions[s];

			auto position = centerPosition + direction.Key.Direction;

			if (!VisitedSpawnPositions.Contains(position))
			{
				VisitedSpawnPositions.Add(position);
				SpawnPositionsArray.Enqueue(position);
			}
		}
		
		//Mesh
		if (FVector::Distance(FVector(initialCenter), FVector(centerPosition)) < SpawnZone)
		{
			GenerateChunkMesh(faceParams, centerPosition);
		}
	}
}

void APreloadedVoxelCenterAreaChunkSpawner::WaitForAllTasks(TArray<TSharedFuture<void>>& tasks)
{
	for (auto task : tasks)
	{
		if (task.IsValid() && !task.IsReady())
		{
			task.Wait();
		}
	}

	tasks.Empty();
}
