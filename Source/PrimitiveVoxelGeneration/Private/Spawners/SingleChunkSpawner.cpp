// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/SingleChunkSpawner.h"

void ASingleChunkSpawner::BeginPlay()
{
	// Check if the template is valid
	if (ChunkMesherBlueprint)
	{
		// Create the component
		ChunkMesherBase = NewObject<UChunkMesherBase>(this, ChunkMesherBlueprint);

		if (ChunkMesherBase)
		{
			// Register the component (required for it to work properly)
			ChunkMesherBase->RegisterComponent();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ChunkTemplate is nullptr!"));
	}
	
	SpawnChunk(SingleChunk, SingleChunkGridPosition);
	
	Super::BeginPlay();
}