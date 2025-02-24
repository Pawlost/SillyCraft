// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/AreaChunkSpawner.h"

void AAreaChunkSpawner::BeginPlay()
{
	Super::BeginPlay();
	ChunkGrid.Reserve(2 * SpawnRadius * SpawnRadius * SpawnRadius);
	CenterGridPosition = WorldPositionToChunkGridPosition(GetTransform().GetLocation());
	GenerateChunks();
}

void AAreaChunkSpawner::AddChunkFromGrid(FChunkFaceParams& params, const FGridDirectionToFace& faceDirection)
{
	auto chunk = ChunkGrid.Find(params.ChunkParams.OriginalChunk->GridPosition + faceDirection.Direction);
	if (chunk == nullptr)
	{
		AddSideChunk(params, faceDirection.FaceSide, nullptr);
	}
	else
	{
		AddSideChunk(params, faceDirection.FaceSide, *chunk);
	}
}

void AAreaChunkSpawner::GenerateChunks()
{
	SpawnHandle = Async(EAsyncExecution::TaskGraph, [this]()
	{
		auto minPosition = CenterGridPosition - FIntVector(SpawnRadius);
		auto maxPosition = CenterGridPosition + FIntVector(SpawnRadius);

		auto minExtendedBorder = minPosition;
		auto maxExtendedBorder = maxPosition;

		if (!ShowBorderChunks)
		{
			auto singleVector = FIntVector(1);
			minExtendedBorder -= singleVector;
			maxExtendedBorder += singleVector;
		}

		for (int32 x = minExtendedBorder.X; x < maxExtendedBorder.X; x++)
		{
			for (int32 y = minExtendedBorder.Y; y < maxExtendedBorder.Y; y++)
			{
				for (int32 z = minExtendedBorder.Z; z < maxExtendedBorder.Z; z++)
				{
					if (!IsValid(this))
					{
						return;
					}

					auto currentGridPosition = FIntVector(x, y, z);
					if (!ChunkGrid.Contains(currentGridPosition))
					{
						auto Chunk = MakeShared<FChunkStruct>().ToSharedPtr();
						Chunk->GridPosition = currentGridPosition;
						ChunkMesher->GenerateVoxels(Chunk);
						SpawnChunk(Chunk);
						ChunkGrid.Add(currentGridPosition, Chunk);
					}
				}
			}
		}

		for (int32 x = minPosition.X; x < maxPosition.X; x++)
		{
			for (int32 y = minPosition.Y; y < maxPosition.Y; y++)
			{
				for (int32 z = minPosition.Z; z < maxPosition.Z; z++)
				{
					auto currentGridPosition = FIntVector(x, y, z);
					if (!IsValid(this))
					{
						return;
					}
					if (ChunkGrid.Contains(currentGridPosition))
					{
						FChunkFaceParams chunkParams;
						chunkParams.ChunkParams.OriginalChunk = *ChunkGrid.Find(currentGridPosition);
						AddChunkFromGrid(chunkParams, FGridDirectionToFace::TopDirection);
						AddChunkFromGrid(chunkParams, FGridDirectionToFace::BottomDirection);
						AddChunkFromGrid(chunkParams, FGridDirectionToFace::RightDirection);
						AddChunkFromGrid(chunkParams, FGridDirectionToFace::LeftDirection);
						AddChunkFromGrid(chunkParams, FGridDirectionToFace::FrontDirection);
						AddChunkFromGrid(chunkParams, FGridDirectionToFace::BackDirection);
						ChunkMesher->GenerateMesh(chunkParams);
					}
				}
			}
		}
	});
}

void AAreaChunkSpawner::DespawnChunks()
{
	AsyncTask(ENamedThreads::AnyThread, [this]()
	{
		SpawnHandle.Wait();
		auto gridCenterPosition = FVector(CenterGridPosition);
		TArray<FIntVector> chunkKeys;
		ChunkGrid.GetKeys(chunkKeys);

		for (auto chunkKey : chunkKeys)
		{
			auto chunkPosition = FVector(chunkKey);

			if (FVector::Distance(chunkPosition, gridCenterPosition) > DespawnRadius)
			{
				if (!IsValid(this))
				{
					return;
				}

				auto chunk = *ChunkGrid.Find(chunkKey);
				auto actorPtr = chunk->ChunkMeshActor;

				AsyncTask(ENamedThreads::GameThread, [this, actorPtr]()
				{
					if (actorPtr.IsValid())
					{
						actorPtr->Destroy();
					}
				});

				ChunkGrid.Remove(chunkKey);
			}
		}
	});
}
