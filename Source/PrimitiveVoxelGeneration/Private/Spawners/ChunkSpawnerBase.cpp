// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawners/ChunkSpawnerBase.h"

void AChunkSpawnerBase::SpawnChunk(FChunkStruct& chunk, const FIntVector& spawnGridPosition)
{
	chunk.GridPosition = spawnGridPosition;
	VoxelGridGenerator->GenerateVoxels(chunk);

	auto spawnLocation = FVector(spawnGridPosition.X, spawnGridPosition.Y, spawnGridPosition.Z) * VoxelGridGenerator->GetVoxelCountY();
	chunk.ChildChunk = GetWorld()->SpawnActor<AChunkRMCActor>(AChunkRMCActor::StaticClass(), spawnLocation, FRotator::ZeroRotator);
	if (chunk.ChildChunk.IsValid())
	{
		chunk.ChildChunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}
}
