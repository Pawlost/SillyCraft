#include "Spawner/Area/PreloadedVoxelCenterAreaChunkSpawner.h"

#include "Mesher/MeshingUtils/MesherVariables.h"

void APreloadedVoxelCenterAreaChunkSpawner::GenerateArea()
{
	auto initialCenter = CenterGridPosition;
	TSet<FIntVector> VisitedSpawnPositions;
	VisitedSpawnPositions.Reserve(SpawnZone * SpawnZone * SpawnZone * CHUNK_FACE_COUNT);
	TQueue<FIntVector> SpawnPositionsArray;
	SpawnChunk(initialCenter);
	TArray<TSharedFuture<void>> tasks;
	tasks.Reserve(6);

	SpawnPositionsArray.Enqueue(initialCenter);

	TTuple<FFaceToDirection, int32> Directions[6] = {
		TTuple<FFaceToDirection, int32>(FFaceToDirection::FrontDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::RightDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::LeftDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::BackDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::TopDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::BottomDirection, SpawnZone)
	};

	FIntVector centerPosition;
	VisitedSpawnPositions.Add(CenterGridPosition);

	while (IsValid(this) && SpawnPositionsArray.Dequeue(centerPosition) && initialCenter == CenterGridPosition)
	{
		for (int32 s = 0; s < CHUNK_FACE_COUNT; s++)
		{
			auto direction = Directions[s];

			auto position = centerPosition + direction.Key.Direction;

			auto chunkPtr = ChunkGrid.Find(position);

			if (FVector::Distance(FVector(initialCenter), FVector(position)) < direction.Value)
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

		if (tasks.Num() >= 6)
		{
			WaitForAllTasks(tasks);
		}
	}

	WaitForAllTasks(tasks);

	FMesherVariables faceParams;
	for (auto VisitedSpawnPosition : VisitedSpawnPositions)
	{
		GenerateChunkMesh(faceParams, VisitedSpawnPosition);
	}
}

void APreloadedVoxelCenterAreaChunkSpawner::BeginPlay()
{
	ShowChunkBorders = true;
	Super::BeginPlay();
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
