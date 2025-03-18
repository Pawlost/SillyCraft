#include "Spawner/Single/SingleChunkSpawnerBase.h"

#include "Voxel/Generators/VoxelGeneratorBase.h"

void ASingleChunkSpawnerBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (AlignGridPositionWithSpawner)
	{
		SingleChunkGridPosition = WorldPositionToChunkGridPosition(GetActorLocation());;
	}

	// Initialize single chunk
	SingleChunk = MakeShared<FChunk>();

	AsyncTask(ENamedThreads::BackgroundThreadPriority, [this]()
	{
		AddChunkToGrid(SingleChunk, SingleChunkGridPosition);
		StartMeshing();
	});
}

void ASingleChunkSpawnerBase::ChangeVoxelInChunk(const FIntVector& ChunkGridPosition, const FIntVector& VoxelPosition,
	const FName& VoxelId)
{
	if (ChunkGridPosition != SingleChunkGridPosition)
	{
		// Return if adding to single chunk border
		return;
	}

	AsyncTask(ENamedThreads::BackgroundThreadPriority, [this, VoxelPosition, VoxelId]()
	{
		FScopeLock Lock(&CritSection);
		
		// Modify voxel at hit position
		VoxelGenerator->ChangeUnknownVoxelIdInChunk(SingleChunk, VoxelPosition, VoxelId);
		StartMeshing();
	});
}