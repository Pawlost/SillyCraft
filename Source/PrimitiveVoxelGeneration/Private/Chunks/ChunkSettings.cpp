// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkSettings.h"

int32 FChunkSettings::GetAxisVoxelCount() const
{
	return AxisVoxelCount;	
}

void FChunkSettings::SetAxisVoxelCount(const int32 chunkSideSizeInBlocks)
{
	this->AxisVoxelCount = chunkSideSizeInBlocks;
	PlaneVoxelCount = chunkSideSizeInBlocks * chunkSideSizeInBlocks;
}

double FChunkSettings::GetVoxelSize() const
{
	return VoxelSize;
}

void FChunkSettings::SetVoxelSize(const double voxelSize)
{
	this->VoxelSize = voxelSize;
}

int32 FChunkSettings::GetChunkSize() const
{
	return AxisVoxelCount * VoxelSize;
}

int32 FChunkSettings::GetVoxelIndex(const int32 x, const int32 y, const int32 z) const
{
	return y + (z * AxisVoxelCount) + (x * PlaneVoxelCount);
}

int32 FChunkSettings::GetVoxelIndex(const FIntVector& indexVector) const
{
	return indexVector.Y + (indexVector.Z * AxisVoxelCount) + (indexVector.X * PlaneVoxelCount);
}

FIntVector3 FChunkSettings::IndexToCoords(const int32 index) const
{
	return FIntVector3(index / AxisVoxelCount, index % AxisVoxelCount, (index / AxisVoxelCount) % AxisVoxelCount);
}

double FChunkSettings::GetMaximumElevation() const
{
	return MaximumElevation / VoxelSize;
}

void FChunkSettings::SetMaximumElevation(const double maximumElevation)
{
	this->MaximumElevation = maximumElevation;
}

int32 FChunkSettings::GetPlaneVoxelCount() const
{
	return PlaneVoxelCount;
}

int32 FChunkSettings::MaxVoxelCount() const
{
	return AxisVoxelCount * AxisVoxelCount *AxisVoxelCount;
}