﻿// Fill out your copyright notice in the Description page of Project Settings.pp[p
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
	TArray<TSharedFuture<void>> tasks;
	tasks.Reserve(6);

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

		if (tasks.Num() >= 6)
		{
			WaitForAllTasks(tasks);
		}
		
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

		//Mesh
		if (FVector::Distance(FVector(initialCenter), FVector(centerPosition)) < SpawnZone)
		{
			WaitForAllTasks(tasks);
			GenerateChunkMesh(faceParams, centerPosition);
		}
	}
}

void ACenterAreaChunkSpawner::WaitForAllTasks(TArray<TSharedFuture<void>>& tasks)
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
