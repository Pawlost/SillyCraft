// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/AreaChunkSpawner.h"

void AAreaChunkSpawner::BeginPlay()
{
	// Check if the template is valid
	if (ChunkMesherTemplate)
	{
		// Create the component

		if (const auto ChunkMesherComponent = NewObject<UChunkMesherBase>(this, ChunkMesherTemplate))
		{
			// Register the component (required for it to work properly)
			ChunkMesherComponent->RegisterComponent();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ChunkTemplate is nullptr!"));
	}
	
	Super::BeginPlay();
}