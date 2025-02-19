// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunks/ChunkGridData.h"
#include "Voxels/VoxelType.h"

void UChunkGridData::AddChunkToGrid(TWeakObjectPtr<UChunkMesherBase> chunk, const FIntVector& gridPos) const
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

TWeakObjectPtr<UChunkMesherBase> UChunkGridData::GetChunkPtr(const FIntVector& gridPos) const
{
	FScopeLock Lock(&GridLock);
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

TSharedPtr<TMap<FIntVector, TWeakObjectPtr<UChunkMesherBase>>> UChunkGridData::GetSpawnedChunks() const
{
	return SpawnedChunks;
}

void UChunkGridData::SetSpawnedChunks(const TSharedPtr<TMap<FIntVector, TWeakObjectPtr<UChunkMesherBase>>>& spawnedChunks)
{
	this->SpawnedChunks = spawnedChunks;
}

void UChunkGridData::SetVoxelTypes(const TWeakObjectPtr<UDataTable>& voxelTypes)
{
	this->VoxelTypes = voxelTypes;
	VoxelIds = VoxelTypes->GetRowNames();
}

int32 UChunkGridData::GetVoxelIdCount() const
{
	if(!IsValid(this)){return 0;}
	
	return VoxelIds.Num();
}

FVoxelType UChunkGridData::GetVoxelTypeById(const int32& voxelId)
{
	auto rowName = VoxelIds[voxelId];

	return *VoxelTypes->FindRow<FVoxelType>(rowName, "");
}

void UChunkGridData::SetChunkSettings(const TSharedPtr<FChunkSettings>& chunkSettings)
{
	ChunkSettings = chunkSettings;
}
