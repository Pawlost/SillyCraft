// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/GenerationSettings.h"


FDataTableRowHandle FGenerationSettings::GetVoxelTypes() const
{
	return VoxelTypes;
}

void FGenerationSettings::SetVoxelTypes(const FDataTableRowHandle& voxelTypes)
{
	this->VoxelTypes = voxelTypes;
}

int64 FGenerationSettings::GetSeed() const
{
	return Seed;
}

void FGenerationSettings::SetSeed(const int64 seed)
{
	this->Seed = seed;
}

int32 FGenerationSettings::GetChunkSideSizeInVoxels() const
{
	return ChunkSideSizeInVoxels;	
}

void FGenerationSettings::SetChunkSizeInVoxels(const int32 chunkSideSizeInBlocks)
{
	this->ChunkSideSizeInVoxels = chunkSideSizeInBlocks;
	ChunkPlaneSizeInVoxels = chunkSideSizeInBlocks * chunkSideSizeInBlocks;
}

int32 FGenerationSettings::GetVoxelSize() const
{
	return VoxelSize;
}

void FGenerationSettings::SetVoxelSize(const int32 voxelSize)
{
	this->VoxelSize = voxelSize;
}

int32 FGenerationSettings::GetVoxelIndex(const int32 x, const int32 y, const int32 z) const
{
	return x + (y * ChunkSideSizeInVoxels) + (z * ChunkPlaneSizeInVoxels);
}

FIntVector3 FGenerationSettings::IndexToCoords(const int32 index) const
{
	return FIntVector3(index / ChunkSideSizeInVoxels, index % ChunkSideSizeInVoxels, (index / ChunkSideSizeInVoxels) % ChunkSideSizeInVoxels);
}

int32 FGenerationSettings::GetMaximumElevation() const
{
	return MaximumElevation;
}

void FGenerationSettings::SetMaximumElevation(const int32 maximumElevation)
{
	this->MaximumElevation = maximumElevation;
}

void FGenerationSettings::SetSpawnedChunks(const TSharedPtr<TMap<FIntVector, AChunkActor*>>& spawnedChunks)
{
	this->SpawnedChunks = spawnedChunks;
}

int32 FGenerationSettings::GetChunkPlaneSizeInVoxels() const
{
	return ChunkPlaneSizeInVoxels;
}
