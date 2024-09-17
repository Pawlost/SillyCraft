// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkGridData.h"

void FChunkGridData::AddChunkToGrid(UChunkBase* chunk, const FIntVector& gridPos) const
{
	//FScopeLock Lock(&DataGuard);
	SpawnedChunks->Add(gridPos, chunk);
}

void FChunkGridData::RemoveChunkFromGrid(const FIntVector& gridPos) const
{
//	FScopeLock Lock(&DataGuard);
	SpawnedChunks->Remove(gridPos);
}

bool FChunkGridData::IsChunkInGrid(const FIntVector& gridPos) const
{
	return SpawnedChunks->Contains(gridPos);
}

UChunkBase* FChunkGridData::GetChunkPtr(const FIntVector& gridPos) const
{
	auto chunk = SpawnedChunks->Find(gridPos);
	if(chunk != nullptr)
	{
		return *chunk;
	}

	return nullptr;
}

void FChunkGridData::SetSpawnedChunks(const TSharedPtr<TMap<FIntVector, UChunkBase*>>& spawnedChunks)
{
	this->SpawnedChunks = spawnedChunks;
}


TSharedPtr<FChunkSettings> FChunkGridData::GetChunkSettings() const
{
	return ChunkSettings;
}

void FChunkGridData::SetChunkSettings(const TSharedPtr<FChunkSettings>& chunKSettings)
{
	ChunkSettings = chunKSettings;
}
