// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkGridData.h"

void UChunkGridData::AddChunkToGrid(TWeakObjectPtr<UChunkBase> chunk, const FIntVector& gridPos) const
{
	FScopeLock Lock(&GridLock);
	
	if(SpawnedChunks.IsValid())
	{
		SpawnedChunks->Add(gridPos, chunk);
	}
}

void UChunkGridData::RemoveChunkFromGrid(const FIntVector& gridPos) const
{
	if(SpawnedChunks.IsValid())
	{
		SpawnedChunks->Remove(gridPos);
	}
}

bool UChunkGridData::IsChunkInGrid(const FIntVector& gridPos) const
{
	if(SpawnedChunks.IsValid())
	{
		return SpawnedChunks->Contains(gridPos);
	}
	
	return false;
}

TWeakObjectPtr<UChunkBase> UChunkGridData::GetChunkPtr(const FIntVector& gridPos) const
{
	if(SpawnedChunks.IsValid())
	{
		auto chunk = SpawnedChunks->Find(gridPos);
		if(chunk != nullptr && chunk->IsValid(false, true))
		{
			return *chunk;
		}
	}
	
	return nullptr;
}

TSharedPtr<FChunkSettings> UChunkGridData::GetChunkSettings() const
{
	return ChunkSettings;
}

TSharedPtr<TMap<FIntVector, TWeakObjectPtr<UChunkBase>>> UChunkGridData::GetSpawnedChunks() const
{
	return SpawnedChunks;
}

void UChunkGridData::SetSpawnedChunks(const TSharedPtr<TMap<FIntVector, TWeakObjectPtr<UChunkBase>>>& spawnedChunks)
{
	this->SpawnedChunks = spawnedChunks;
}

void UChunkGridData::SetVoxelTypes(const TSharedPtr<TArray<TWeakFieldPtr<FVoxelType>>>& voxelTypes)
{
	this->VoxelTypes = voxelTypes;
}

int32 UChunkGridData::GetVoxelTypeNum() const
{
	return VoxelTypes.Get()->Num();
}

void UChunkGridData::SetChunkSettings(const TSharedPtr<FChunkSettings>& chunkSettings)
{
	ChunkSettings = chunkSettings;
}
