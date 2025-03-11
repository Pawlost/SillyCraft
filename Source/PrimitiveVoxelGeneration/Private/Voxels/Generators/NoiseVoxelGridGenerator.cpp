// Fill out your copyright notice in the Description page of Project Settings.
#include "Voxels/VoxelGenerators/NoiseVoxelGridGenerator.h"

#include "FastNoiseWrapper.h"
#include "Voxels/Voxel.h"

void UNoiseVoxelGridGenerator::BeginPlay()
{
	Super::BeginPlay();

	const int voxelTypeCount = GetVoxelTypeCount();

	for (int voxelId = 0; voxelId < voxelTypeCount; voxelId++)
	{
		// Create the component dynamically
		TObjectPtr<UFastNoiseWrapper> noise = NewObject<UFastNoiseWrapper>(this);
		const FVoxelType voxelType = GetVoxelTypeById(voxelId);
		noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, voxelType.Seed, voxelType.NoiseFrequency);
		NoiseGenerators.Add(noise);
	}
}


void UNoiseVoxelGridGenerator::GenerateVoxels(FChunkStruct& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	const auto chunkLenght = GetVoxelDimensionCount();
	auto maxElevation = MaxElevation / GetVoxelSize();
	const int voxelTypeCount = GetVoxelTypeCount();

	const auto gridPos = chunk.GridPosition * chunkLenght;

	for (int x = 0; x < chunkLenght; x++)
	{
		for (int y = 0; y < chunkLenght; y++)
		{
			for (int z = 0; z < chunkLenght; z++)
			{
				const auto index = GetVoxelIndex(x, y, z);
				chunk.Voxels[index] = FVoxel();
				for (int voxelId = 0; voxelId < voxelTypeCount; voxelId++)
				{
					auto voxel = FVoxel(voxelId);

					auto noise = NoiseGenerators[voxelId];

					const auto elevation = noise->GetNoise2D(x + gridPos.X, y + gridPos.Y) * maxElevation;

					if (gridPos.Z + z <= elevation)
					{
						AddVoxelAtIndex(chunk, index, voxel);
					}
				}
			}
		}
	}
}

double UNoiseVoxelGridGenerator::GetHighestElevationAtLocation(const FVector& location)
{
	double maxElevation = 0.0;

	auto maxElevationConstant = MaxElevation / GetVoxelSize();

	for (int voxelId = 0; voxelId < GetVoxelTypeCount(); voxelId++)
	{
		auto noise = NoiseGenerators[voxelId];
		double elevation = noise->GetNoise2D(location.X, location.Y) * maxElevationConstant;

		if (elevation > maxElevation)
		{
			maxElevation = elevation;
		}
	}

	return maxElevation * VoxelSize;
}
