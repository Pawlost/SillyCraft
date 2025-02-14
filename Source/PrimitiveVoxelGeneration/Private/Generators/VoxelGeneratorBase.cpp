// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/VoxelGeneratorBase.h"

#include "Voxels/VoxelType.h"

UVoxelGeneratorBase::UVoxelGeneratorBase()
{
	VoxelCountY *= VoxelSize;
	VoxelCountYZ = VoxelCountY * VoxelCountY;
	VoxelCountXYZ = VoxelCountYZ * VoxelCountY;
	MaxElevation /= VoxelSize;
}


int32 UVoxelGeneratorBase::GetVoxelIndex(const int32 x, const int32 y, const int32 z) const
{
	return y + (z * VoxelCountY) + (x * VoxelCountYZ);
}

int32 UVoxelGeneratorBase::GetVoxelCountY() const
{
	return VoxelCountY;
}

int32 UVoxelGeneratorBase::GetVoxelCountYZ() const
{
	return VoxelCountYZ;
}

int32 UVoxelGeneratorBase::GetVoxelCountXYZ() const
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
