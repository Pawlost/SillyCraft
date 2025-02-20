// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/VoxelGeneratorBase.h"

#include "Voxels/VoxelType.h"

UVoxelGeneratorBase::UVoxelGeneratorBase()
{
	Noise = CreateDefaultSubobject<UFastNoiseWrapper>("NoiseGenerator");
}

int32 UVoxelGeneratorBase::GetVoxelIndex(const int32 x, const int32 y, const int32 z) const
{
	return y + (z * VoxelCountY) + (x * VoxelCountYZ);
}

double UVoxelGeneratorBase::GetChunkSize()
{
	return ChunkSize;
}

int32 UVoxelGeneratorBase::GetVoxelDimensionCount() const
{
	return VoxelDimensionCount;
}

double_t UVoxelGeneratorBase::GetVoxelSize() const
{
	return VoxelSize;
}

int32 UVoxelGeneratorBase::GetVoxel2DimensionCount() const
{
	return VoxelCountYZ;
}

int32 UVoxelGeneratorBase::GetVoxel3DimensionCount() const
{
	return VoxelCountXYZ;
}

double_t UVoxelGeneratorBase::GetMaximumElevation() const
{
	return MaxElevation;
}

int32 UVoxelGeneratorBase::GetVoxelTypeCount() const
{
	return VoxelTypeTable->GetRowNames().Num();
}

FVoxelType UVoxelGeneratorBase::GetVoxelTypeById(const int32& voxelTypeIndex) const
{
	auto rowName = VoxelTypeTable->GetRowNames()[voxelTypeIndex];

	return *VoxelTypeTable->FindRow<FVoxelType>(rowName, "");
}

int32 UVoxelGeneratorBase::GetVoxelIndex(const FIntVector& indexVector) const
{
	return GetVoxelIndex(indexVector.X, indexVector.Y, indexVector.Z);
}

void UVoxelGeneratorBase::BeginPlay()
{
	VoxelCountY = VoxelDimensionCount;
	ChunkSize = VoxelCountY * VoxelSize;
	VoxelCountYZ = VoxelCountY * VoxelCountY;
	VoxelCountXYZ = VoxelCountYZ * VoxelCountY;
	MaxElevation /= VoxelSize;
	Super::BeginPlay();
}

//
// double UVoxelGeneratorBase::GetHighestElevationAtPosition(double posX, double posY)
// {
// 	double maxElevation = 0.0;
//
// 	auto voxelSize = ChunkSettings->GetVoxelSize();
// 	
// 	int voxelIds = ChunkGridData->GetVoxelIdCount();
//
// 	auto voxelPosX = posX / voxelSize;
// 	auto voxelPosY = posY / voxelSize;
// 	
// 	for(int voxelId = 0; voxelId < voxelIds; voxelId++)
// 	{
// 		FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(voxelId);
// 		Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, voxelType.Seed,  voxelType.NoiseFrequency);
// 		double elevation = Noise->GetNoise2D(voxelPosX, voxelPosY) * ChunkSettings->GetMaximumElevation();
//
// 		if(elevation > maxElevation)
// 		{
// 			maxElevation = elevation;
// 		}
// 	}
// 	
// 	return maxElevation * voxelSize;
// }
