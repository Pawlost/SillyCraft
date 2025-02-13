// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawners/ChunkSpawnerBase.h"

#include "Chunks/ChunkRMCActor.h"

void AChunkSpawnerBase::SpawnChunk(FChunkStruct& chunk, const FVector& location)
{
	chunk.ChildChunk = GetWorld()->SpawnActor<AChunkRMCActor>(AChunkRMCActor::StaticClass(), location, FRotator::ZeroRotator);
	if (chunk.ChildChunk.IsValid())
	{
		chunk.ChildChunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}
}
