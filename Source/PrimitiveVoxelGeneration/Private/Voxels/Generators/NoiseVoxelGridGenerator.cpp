// Fill out your copyright notice in the Description page of Project Settings.
#include "Voxels/VoxelGenerators/NoiseVoxelGridGenerator.h"

#include "FastNoiseWrapper.h"
#include "Voxels/Voxel.h"

UNoiseVoxelGridGenerator::UNoiseVoxelGridGenerator()
{
	Noise = CreateDefaultSubobject<UFastNoiseWrapper>("NoiseGenerator");
}

void UNoiseVoxelGridGenerator::GenerateVoxels(TSharedPtr<FChunkStruct>& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	const auto chunkLenght = GetVoxelDimensionCount();
	auto maxElevation = MaxElevation / GetVoxelSize();

	const auto gridPos = chunk->GridPosition * chunkLenght;
	const int voxelTypeCount = GetVoxelTypeCount();
	chunk->ChunkVoxelTypeTable.Reserve(voxelTypeCount);
	chunk->Voxels.SetNum(GetVoxel3DimensionCount());

	for (int voxelId = 0; voxelId < voxelTypeCount; voxelId++)
	{
		auto voxel = FVoxel(voxelId);
		SetupNoiseByVoxelId(voxelId);
		
		for (int x = 0; x < chunkLenght; x++)
		{
			for (int y = 0; y < chunkLenght; y++)
			{
				const auto elevation = Noise->GetNoise2D(x + gridPos.X, y + gridPos.Y) * maxElevation;

				for (int z = 0; z < chunkLenght; z++)
				{
					const auto index = GetVoxelIndex(x, y, z);

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

void UNoiseVoxelGridGenerator::SetupNoiseByVoxelId(int voxelId) const
{
	const FVoxelType voxelType = GetVoxelTypeById(voxelId);
	Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, voxelType.Seed, voxelType.NoiseFrequency);
}
