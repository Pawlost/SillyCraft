#include "Spawner/Single/SingleChunkSpawnerBase.h"

void ASingleChunkSpawnerBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (AlignGridPositionWithSpawner)
	{
		SingleChunkGridPosition = WorldPositionToChunkGridPosition(GetActorLocation());;
	}
	
	SingleChunk = MakeShared<FChunk>();

	AsyncTask(ENamedThreads::AnyThread, [this]()
	{
		InitChunk(SingleChunk, SingleChunkGridPosition);
		StartMeshing();
	});
}

void ASingleChunkSpawnerBase::ChangeVoxelInChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition,
	const FName& VoxelId)
{
	if (chunkGridPosition != SingleChunkGridPosition)
	{
		return;
	}
	
	VoxelGenerator->ChangeUnknownVoxelIdInChunk(SingleChunk, voxelPosition, VoxelId);
	StartMeshing();
}