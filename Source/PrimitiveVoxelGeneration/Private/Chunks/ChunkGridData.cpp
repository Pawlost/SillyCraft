// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkGridData.h"


void FChunkGridData::AddChunkToGrid(const FIntVector& gridPos, AChunkActor* actor) const
{
	//FScopeLock Lock(&DataGuard);
	SpawnedChunks->Add(gridPos, actor);
}

void FChunkGridData::RemoveChunkFromGrid(const FIntVector& gridPos) const
{
//	FScopeLock Lock(&DataGuard);
	SpawnedChunks->Remove(gridPos);
}

bool FChunkGridData::IsChunkInGrid(const FIntVector& gridPos) const
{
	//FScopeLock Lock(&DataGuard);
	return  SpawnedChunks->Contains(gridPos);
}

void FChunkGridData::SetSpawnedChunks(const TSharedPtr<TMap<FIntVector, AChunkActor*>>& spawnedChunks)
{
	this->SpawnedChunks = spawnedChunks;
}

TSharedPtr<FChunkSettings> FChunkGridData::GetChunkSettings() const
{
	return ChunkSettings;
}

TSubclassOf<UChunkBase> FChunkGridData::GetChunkClass() const
{
	return ChunkClass;
}

void FChunkGridData::SetChunkClass(const TSubclassOf<UChunkBase>& chunkClass)
{
	this->ChunkClass = chunkClass;
}

void FChunkGridData::SetChunkSettings(const TSharedPtr<FChunkSettings>& chunKSettings)
{
	ChunkSettings = chunKSettings;
}
