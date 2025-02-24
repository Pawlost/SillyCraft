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

void AChunkSpawnerBase::MoveSpawnToPosition(const FVector& newPosition)
{
	auto newGridPosition = WorldPositionToChunkGridPosition(newPosition);

	if (CenterGridPosition != newGridPosition)
	{
		CenterGridPosition = newGridPosition;
		GenerateChunks();
		DespawnChunks();
	}
}

double AChunkSpawnerBase::GetHighestElevationAtLocation(const FVector& location) const
{
	return ChunkMesher->GetHighestElevationAtLocation(location);
}

TFuture<void> AChunkSpawnerBase::SpawnChunk(const TSharedPtr<FChunkStruct>& chunk)
{
	return Async(EAsyncExecution::TaskGraphMainThread, [this, chunk]()
	{
		auto world = GetWorld();
		if (!IsValid(world))
		{
			return;
		}

		auto spawnLocation = FVector(chunk->GridPosition.X, chunk->GridPosition.Y, chunk->GridPosition.Z) * ChunkMesher
			->GetChunkSize();

		chunk->ChunkMeshActor = world->SpawnActor<AChunkRmcActor>(AChunkRmcActor::StaticClass(), spawnLocation,
		                                                          FRotator::ZeroRotator);
		if (chunk->ChunkMeshActor.IsValid())
		{
			chunk->ChunkMeshActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
	});
}

void AChunkSpawnerBase::AddSideChunk(FChunkFaceParams& chunkParams, EFaceDirection direction,
                                     const TSharedPtr<FChunkStruct>& chunk)
{
	auto directionIndex = static_cast<uint8>(direction);
	chunkParams.ChunkParams.SideChunks[directionIndex] = chunk.IsValid() ? chunk : nullptr;
}

FIntVector AChunkSpawnerBase::WorldPositionToChunkGridPosition(const FVector& worldPosition) const
{
	auto location = worldPosition / ChunkMesher->GetChunkSize();
	return FIntVector(FMath::FloorToInt32(location.X), FMath::FloorToInt32(location.Y),
	                  FMath::FloorToInt32(location.Z));
}
