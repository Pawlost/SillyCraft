// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkSettings.h"

FDataTableRowHandle FUChunkSettings::GetVoxelTypes() const
{
	return VoxelTypes;
}

void FUChunkSettings::SetVoxelTypes(const FDataTableRowHandle& voxelTypes)
{
	this->VoxelTypes = voxelTypes;
}

int64 FUChunkSettings::GetSeed() const
{
	return Seed;
}

void FUChunkSettings::SetSeed(const int64 seed)
{
	this->Seed = seed;
}

int32 FUChunkSettings::GetChunkWidthInBlocks() const
{
	return ChunkWidthInBlocks;
}

void FUChunkSettings::SetChunkWidthInBlocks(const int32 chunkWidthInBlocks)
{
	this->ChunkWidthInBlocks = chunkWidthInBlocks;
}

int32 FUChunkSettings::GetVoxelSize() const
{
	return VoxelSize;
}

void FUChunkSettings::SetVoxelSize(const int32 voxelSize)
{
	this->VoxelSize = voxelSize;
}
