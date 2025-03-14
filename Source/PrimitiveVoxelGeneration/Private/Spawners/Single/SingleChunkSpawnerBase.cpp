// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawners/Single/SingleChunkSpawnerBase.h"

void ASingleChunkSpawnerBase::BeginPlay()
{
	Super::BeginPlay();

	if (AlignGridPositionWithSpawner)
	{
		SingleChunkGridPosition = WorldPositionToChunkGridPosition(GetActorLocation());;
	}
	
	SingleChunk = MakeShared<FChunkStruct>();

	AsyncTask(ENamedThreads::AnyThread, [this]()
	{
		InitChunk(SingleChunk, SingleChunkGridPosition);
		StartMeshing();
	});
}

void ASingleChunkSpawnerBase::ChangeVoxelInChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition,
	const FName& VoxelId)
{
	VoxelGenerator->ChangeVoxelIdInChunk(SingleChunk, voxelPosition, VoxelId);
	StartMeshing();
}