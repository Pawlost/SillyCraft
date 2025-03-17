// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawner/Area/CenterAreaChunkSpawner.h"

#include "Mesher/MeshingUtils/MesherVariables.h"

void ACenterAreaChunkSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void ACenterAreaChunkSpawner::GenerateArea()
{
	auto initialCenter = CenterGridPosition;
	TSet<FIntVector> VisitedSpawnPositions;
	VisitedSpawnPositions.Reserve(SpawnZone * SpawnZone * SpawnZone * CHUNK_FACE_COUNT);
	TQueue<FIntVector> SpawnPositionsArray;
	SpawnPositionsArray.Enqueue(initialCenter);
	SpawnChunk(initialCenter);
	FMesherVariables faceParams;
	faceParams.ChunkParams.ShowBorders = BufferZone == 0;
	TArray<TSharedFuture<void>> tasks;
	tasks.Reserve(6);

	TTuple<FFaceToDirection, int32> Directions[6] = {
		TTuple<FFaceToDirection, int32>(FFaceToDirection::FrontDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::RightDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::LeftDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::BackDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::TopDirection, ChunksAboveSpawner),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::BottomDirection, ChunksBelowSpawner)
	};

	FIntVector centerPosition;
	
	if (SpawnCenterChunk)
	{
		VisitedSpawnPositions.Add(CenterGridPosition);
	}
	
	while (IsValid(this) && SpawnPositionsArray.Dequeue(centerPosition) && initialCenter == CenterGridPosition)
	{

		if (tasks.Num() >= 6)
		{
			WaitForAllTasks(tasks);
		}
		
		for (int32 s = 0; s < CHUNK_FACE_COUNT; s++)
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

				if (VisitedSpawnPositions.Find(position) == nullptr)
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
