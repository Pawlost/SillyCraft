// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawners/ChunkSpawnerBase.h"

void AChunkSpawnerBase::BeginPlay()
{
	// Check if the template is valid
	if (VoxelGeneratorBlueprint)
	{
		// Create the component
		VoxelGenerator = NewObject<UVoxelGeneratorBase>(this, VoxelGeneratorBlueprint);

		if (VoxelGenerator)
		{
			// Register the component (required for it to work properly)
			VoxelGenerator->RegisterComponent();
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
	return VoxelGenerator->GetHighestElevationAtLocation(location);
}

void AChunkSpawnerBase::ChangeVoxelAt(const FVector& hitPosition, const FVector& hitNormal, const FVoxel& VoxelId,
                                      bool place)
{
	FVector adjustedNormal;
	if (place)
	{
		adjustedNormal.X = -FMath::Clamp(hitNormal.X, -1, 0);
		adjustedNormal.Y = -FMath::Clamp(hitNormal.Y, -1, 0);
		adjustedNormal.Z = -FMath::Clamp(hitNormal.Z, -1, 0);
	}
	else
	{
		adjustedNormal.X = FMath::Clamp(hitNormal.X, 0, 1);
		adjustedNormal.Y = FMath::Clamp(hitNormal.Y, 0, 1);
		adjustedNormal.Z = FMath::Clamp(hitNormal.Z, 0, 1);
	}

	auto position = hitPosition - adjustedNormal * VoxelGenerator->GetVoxelSize();
	auto chunkGridPosition = WorldPositionToChunkGridPosition(position);

	auto voxelPosition = FIntVector(
		(position - FVector(chunkGridPosition * VoxelGenerator->GetChunkSize())) / VoxelGenerator
		->GetVoxelSize());

	ModifyVoxelAtChunk(chunkGridPosition, voxelPosition, VoxelId);
}

TFuture<TWeakObjectPtr<AChunkRmcActor>> AChunkSpawnerBase::SpawnChunkActor(const FIntVector& gridPosition,
                                                                      TWeakObjectPtr<AChunkRmcActor> ActorPtr)
{
	auto spawnLocation = FVector(gridPosition) * VoxelGenerator->GetChunkSize();
	if (ActorPtr != nullptr)
	{
		return Async(EAsyncExecution::TaskGraphMainThread,
		             [ActorPtr, spawnLocation]() -> TWeakObjectPtr<AChunkRmcActor>
		             {
			             ActorPtr->SetActorLocation(spawnLocation);
			             return ActorPtr;
		             });
	}
	
	auto world = GetWorld();
	return Async(EAsyncExecution::TaskGraphMainThread, [this, world, spawnLocation]() -> TWeakObjectPtr<AChunkRmcActor>
	{
		TWeakObjectPtr<AChunkRmcActor> ActorPtr = nullptr;
		if (!IsValid(world))
		{
			return ActorPtr;
		}

		ActorPtr = world->SpawnActor<AChunkRmcActor>(AChunkRmcActor::StaticClass(), spawnLocation,
		                                             FRotator::ZeroRotator);

		if (ActorPtr.IsValid())
		{
			ActorPtr->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}

		return ActorPtr;
	});
}

void AChunkSpawnerBase::AddSideChunk(FChunkFaceParams& chunkParams, EFaceDirection direction,
                                     const TSharedPtr<FChunkStruct>& chunk)
{
	auto directionIndex = static_cast<uint8>(direction);
	chunkParams.ChunkParams.SideChunks[directionIndex] = chunk.IsValid() ? chunk : nullptr;
}

void AChunkSpawnerBase::InitChunk(TSharedPtr<FChunkStruct>& chunk,
                                  const FIntVector& gridPosition) const
{
	//Actor must always be respawned because of stationary mesh

	chunk->GridPosition = gridPosition;

	if (!chunk->IsInitialized)
	{
		chunk->Voxels.SetNum(VoxelGenerator->GetVoxel3DimensionCount());
		chunk->ChunkVoxelTypeTable.Reserve(VoxelGenerator->GetVoxelTypeCount());
	}

	chunk->IsInitialized = true;
	VoxelGenerator->GenerateVoxels(chunk);
}

FIntVector AChunkSpawnerBase::WorldPositionToChunkGridPosition(const FVector& worldPosition) const
{
	auto location = worldPosition / VoxelGenerator->GetChunkSize();
	return FIntVector(FMath::FloorToInt32(location.X), FMath::FloorToInt32(location.Y),
	                  FMath::FloorToInt32(location.Z));
}
