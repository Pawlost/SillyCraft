// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawners/ChunkSpawnerBase.h"

#include "Meshers/MeshingStructs/ChunkFaceParams.h"
#include "Meshers/MeshingStructs/FaceDirection.h"

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

double AChunkSpawnerBase::GetHighestElevationAtLocation(const FVector& location) const
{
	return VoxelGenerator->GetHighestElevationAtLocation(location);
}

void AChunkSpawnerBase::ChangeVoxelAtHit(const FVector& hitPosition, const FVector& hitNormal, const FName& VoxelName,
                                      bool pick)
{
	FVector adjustedNormal;
	if (pick)
	{
		adjustedNormal.X = FMath::Clamp(hitNormal.X, 0, 1);
		adjustedNormal.Y = FMath::Clamp(hitNormal.Y, 0, 1);
		adjustedNormal.Z = FMath::Clamp(hitNormal.Z, 0, 1);
	}
	else
	{
		adjustedNormal.X = -FMath::Clamp(hitNormal.X, -1, 0);
		adjustedNormal.Y = -FMath::Clamp(hitNormal.Y, -1, 0);
		adjustedNormal.Z = -FMath::Clamp(hitNormal.Z, -1, 0);
	}
	
	auto position = hitPosition - adjustedNormal * VoxelGenerator->GetVoxelSize();
	
	if (!WorldCenter)
	{
		position -= GetActorLocation();	
	}
	
	auto chunkGridPosition = WorldPositionToChunkGridPosition(position);
	
	auto voxelPosition = FIntVector(
		(position - FVector(chunkGridPosition * VoxelGenerator->GetChunkSize())) / VoxelGenerator
		->GetVoxelSize());

	ChangeVoxelInChunk(chunkGridPosition, voxelPosition, VoxelName);
}

void AChunkSpawnerBase::AddSideChunk(FChunkFaceParams& chunkParams, EFaceDirection direction,
                                     const TSharedPtr<FChunkStruct>& chunk)
{
	auto directionIndex = static_cast<uint8>(direction);
	chunkParams.ChunkParams.SideChunks[directionIndex] = chunk.IsValid() ? chunk : nullptr;
}

void AChunkSpawnerBase::InitChunk(TSharedPtr<FChunkStruct>& chunk,
                                  const FIntVector& gridPosition, TSharedFuture<void>* asyncExecution) const
{
	//Actor must always be respawned because of stationary mesh

	chunk->GridPosition = gridPosition;

	if (!chunk->IsInitialized)
	{
		chunk->Voxels.SetNum(VoxelGenerator->GetVoxel3DimensionCount());
	}

	chunk->IsInitialized = true;
	if (asyncExecution != nullptr)
	{
		*asyncExecution = Async(EAsyncExecution::LargeThreadPool, [this, chunk]()
		{
			VoxelGenerator->GenerateVoxels(*chunk.Get());
		}).Share();

	}else
	{
		VoxelGenerator->GenerateVoxels(*chunk);
	}
}

FIntVector AChunkSpawnerBase::WorldPositionToChunkGridPosition(const FVector& worldPosition) const
{
	auto location = worldPosition / VoxelGenerator->GetChunkSize();
	return FIntVector(FMath::FloorToInt32(location.X), FMath::FloorToInt32(location.Y),
	                  FMath::FloorToInt32(location.Z));
}
