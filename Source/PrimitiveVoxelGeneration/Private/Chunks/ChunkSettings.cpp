// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkSettings.h"


FDataTableRowHandle FChunkSettings::GetVoxelTypes() const
{
	return VoxelTypes;
}

void FChunkSettings::SetVoxelTypes(const FDataTableRowHandle& voxelTypes)
{
	this->VoxelTypes = voxelTypes;
}

int64 FChunkSettings::GetSeed() const
{
	return Seed;
}

void FChunkSettings::SetSeed(const int64 seed)
{
	this->Seed = seed;
}

int32 FChunkSettings::GetChunkSideSizeInVoxels() const
{
	return ChunkSideSizeInVoxels;	
}

void FChunkSettings::SetChunkSizeInVoxels(const int32 chunkSideSizeInBlocks)
{
	this->ChunkSideSizeInVoxels = chunkSideSizeInBlocks;
	ChunkPlaneSizeInVoxels = chunkSideSizeInBlocks * chunkSideSizeInBlocks;
}

int32 FChunkSettings::GetVoxelSize() const
{
	return VoxelSize;
}

void FChunkSettings::SetVoxelSize(const int32 voxelSize)
{
	this->VoxelSize = voxelSize;
}

int32 FChunkSettings::GetChunkSize() const
{
	return ChunkSideSizeInVoxels * VoxelSize;
}

int32 FChunkSettings::GetVoxelIndex( const int32 x, const int32 y, const int32 z) const
{
	return y + (z * ChunkSideSizeInVoxels) + (x * ChunkPlaneSizeInVoxels);
}

FIntVector3 FChunkSettings::IndexToCoords(const int32 index) const
{
	return FIntVector3(index / ChunkSideSizeInVoxels, index % ChunkSideSizeInVoxels, (index / ChunkSideSizeInVoxels) % ChunkSideSizeInVoxels);
}

int32 FChunkSettings::GetMaximumElevation() const
{
	return MaximumElevation;
}

void FChunkSettings::SetMaximumElevation(const int32 maximumElevation)
{
	this->MaximumElevation = maximumElevation;
}

int32 FChunkSettings::GetChunkPlaneSizeInVoxels() const
{
	return ChunkPlaneSizeInVoxels;
}

double FChunkSettings::GetNoiseFrequency() const
{
	return NoiseFrequency;
}

void FChunkSettings::SetNoiseFrequency(const double noiseFrequency)
{
	this->NoiseFrequency = noiseFrequency;
}
