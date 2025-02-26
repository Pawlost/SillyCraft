// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxels/VoxelGeneratorBase.h"

#include "Voxels/VoxelType.h"
#include "Chunks/VoxelIdInChunk.h"

void UVoxelGeneratorBase::AddVoxelAtIndex(const TSharedPtr<FChunkStruct>& chunk, const uint32& index, const FVoxel& voxel)
{
	chunk->Voxels[index] = voxel;
	if (!chunk->ChunkVoxelTypeTable.Contains(voxel.VoxelId))
	{
		chunk->ChunkVoxelTypeTable.Add(voxel.VoxelId, FVoxelIdInChunk(chunk->ChunkVoxelTypeTable.Num(), 1));
	}else
	{
		chunk->ChunkVoxelTypeTable.Find(voxel.VoxelId)->CountInChunk ++;
	}
	chunk->IsEmpty = false;
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

double_t UVoxelGeneratorBase::GetVoxelSize()
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

double UVoxelGeneratorBase::GetHighestElevationAtLocation(const FVector& location)
{
	double maxElevation = 0.0;

	for (int voxelId = 0; voxelId < GetVoxelTypeCount(); voxelId++)
	{
		SetupNoiseByVoxelId(voxelId);

		double elevation = Noise->GetNoise2D(location.X, location.Y) * MaxElevation;

		if (elevation > maxElevation)
		{
			maxElevation = elevation;
		}
	}

	return maxElevation * VoxelSize;
}

int32 UVoxelGeneratorBase::GetVoxelIndex(const FIntVector& indexVector) const
{
	return GetVoxelIndex(indexVector.X, indexVector.Y, indexVector.Z);
}

void UVoxelGeneratorBase::SetupNoiseByVoxelId(int voxelId) const
{
	const FVoxelType voxelType = GetVoxelTypeById(voxelId);
	Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, voxelType.Seed, voxelType.NoiseFrequency);
}
