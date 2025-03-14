﻿// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/Area/RealTimeChunkSpawnerBase.h"

#include "Spawners/Area/AreaChunkSpawnerBase.h"

void ARealTimeChunkSpawnerBase::MoveGridCenterToPosition(const FVector& newPosition)
{
	auto newGridPosition = WorldPositionToChunkGridPosition(newPosition);

	if (CenterGridPosition != newGridPosition)
	{
		CenterGridPosition = newGridPosition;
		SpawnChunks();
		DespawnChunks();
	}
}

void ARealTimeChunkSpawnerBase::BeginPlay()
{
	ShowChunkBorders = BufferZone == 0;
	Super::BeginPlay();
}

void ARealTimeChunkSpawnerBase::DespawnChunks()
{
	AsyncTask(ENamedThreads::BackgroundThreadPriority, [this]()
	{
		TArray<FIntVector> chunkKeys;
		ChunkGrid.GetKeys(chunkKeys);

		for (auto chunkKey : chunkKeys)
		{
			if (FVector::Distance(FVector(CenterGridPosition), FVector(chunkKey)) > SpawnZone + DespawnZone)
			{
				if (!IsValid(this) || !ChunkGrid.Contains(chunkKey))
				{
					return;
				}

				auto chunk = *ChunkGrid.Find(chunkKey);

				if (chunk->ChunkMeshActor != nullptr)
				{
					chunk->ChunkMeshActor->ClearMesh();
					UnusedActors.Enqueue(chunk->ChunkMeshActor);
					chunk->ChunkMeshActor = nullptr;
				}

				chunk->IsActive = false;
				chunk->ChunkVoxelTypeTable.Reset();

				m_CriticalSection.Lock();
				ChunkGrid.Remove(chunkKey);
				m_CriticalSection.Unlock();

				DespawnedChunks.Enqueue(chunk);
			}
		}
	});
}
