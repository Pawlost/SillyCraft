// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/AreaGenerator.h"
#include "Chunks/ChunkRMCActor.h"

void AAreaGenerator::SpawnChunk(FVector location)
{
	const auto NewChunk = GetWorld()->SpawnActor<AChunkRMCActor>(AChunkRMCActor::StaticClass(), location, FRotator::ZeroRotator);
	if (NewChunk)
	{
		NewChunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void AAreaGenerator::BeginPlay()
{
	// Check if the template is valid
	if (ChunkMesherTemplate)
	{
		// Create the component
		auto ChunkMesherComponent = NewObject<UChunkMesherBase>(this, ChunkMesherTemplate);

		if (ChunkMesherComponent)
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