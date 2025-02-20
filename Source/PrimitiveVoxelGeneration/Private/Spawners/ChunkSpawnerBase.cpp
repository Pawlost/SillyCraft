// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawners/ChunkSpawnerBase.h"

void AChunkSpawnerBase::BeginPlay()
{
	// Check if the template is valid
	if (MesherBlueprint)
	{
		// Create the component
		ChunkMesher = NewObject<UMesherBase>(this, MesherBlueprint);

		if (ChunkMesher)
		{
			// Register the component (required for it to work properly)
			ChunkMesher->RegisterComponent();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ChunkTemplate is nullptr!"));
	}

	Super::BeginPlay();
}

void AChunkSpawnerBase::SpawnChunk(FChunkStruct& chunk, const FIntVector& spawnGridPosition)
{
	chunk.GridPosition = spawnGridPosition;
	ChunkMesher->GenerateVoxels(chunk);

	auto spawnLocation = FVector(spawnGridPosition.X, spawnGridPosition.Y, spawnGridPosition.Z) * ChunkMesher->
		GetChunkSize();
	chunk.ChildChunk = GetWorld()->SpawnActor<AChunkRMCActor>(AChunkRMCActor::StaticClass(), spawnLocation,
	                                                          FRotator::ZeroRotator);
	if (chunk.ChildChunk.IsValid())
	{
		chunk.ChildChunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void AChunkSpawnerBase::AddSideChunk(FChunkFaceParams& chunkParams, EFaceDirection direction, FChunkStruct* chunk)
{
	auto directionIndex = static_cast<uint8>(direction);
	chunkParams.ChunkParams.SideChunks[directionIndex] = chunk;
}
