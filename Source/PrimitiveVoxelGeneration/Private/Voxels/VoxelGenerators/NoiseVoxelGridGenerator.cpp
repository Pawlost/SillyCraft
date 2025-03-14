﻿// Fill out your copyright notice in the Description page of Project Settings.
#include "Voxels/VoxelGenerators/NoiseVoxelGridGenerator.h"

#include "FastNoiseWrapper.h"
#include "Voxels/Voxel.h"

void UNoiseVoxelGridGenerator::BeginPlay()
{
	Super::BeginPlay();

	for (auto voxelName : VoxelTypeTable->GetRowNames())
	{
		auto voxelType = *VoxelTypeTable->FindRow<FVoxelType>(voxelName, "");
		
		// Create the component dynamically
		TObjectPtr<UFastNoiseWrapper> surfaceNoise = NewObject<UFastNoiseWrapper>(this);
		TObjectPtr<UFastNoiseWrapper> reversedSurfaceNoise = NewObject<UFastNoiseWrapper>(this);
		
		surfaceNoise->SetupFastNoise(voxelType.SurfaceNoise_Type, voxelType.SurfaceNoise_Seed,
									 voxelType.SurfaceNoise_Frequency);
			
		reversedSurfaceNoise->SetupFastNoise(voxelType.ReversedSurfaceNoise_Type,
											 voxelType.ReversedSurfaceNoise_Seed,
											 voxelType.ReversedSurfaceNoise_Frequency);

		auto surfaceGenerator = FNoiseSurfaceGenerator();
		surfaceGenerator.VoxelName = voxelName;
		surfaceGenerator.VoxelType = voxelType;

		surfaceGenerator.SurfaceGenerator = surfaceNoise;
		surfaceGenerator.ReverseSurfaceGenerator = reversedSurfaceNoise;
		SurfaceGenerators.Add(surfaceGenerator);
	}
}

FVoxel UNoiseVoxelGridGenerator::GetVoxelByName(const FName& voxelName) const
{
	for (int32 Index = 0; Index < SurfaceGenerators.Num(); Index++)
	{
		auto surfaceGenerator = SurfaceGenerators[Index];
		if (surfaceGenerator.VoxelName == voxelName)
		{
			return FVoxel(Index, surfaceGenerator.VoxelType.bIsTransparent);
		}
	}
	return FVoxel();
}

double UNoiseVoxelGridGenerator::GetSurfaceGradient(float posX, float posY, const TObjectPtr<UFastNoiseWrapper>& generator, const FVoxelType& voxelType)
{
	return generator->GetNoise2D(posX, posY) * voxelType.Surface_Elevation + voxelType.Surface_DistanceFromSeaLevel;
}

bool UNoiseVoxelGridGenerator::IsChunkPositionOutOfBounds(double minZPosition, double maxZPosition)
{
	for (auto generator : SurfaceGenerators)
	{
		if (minZPosition < generator.VoxelType.Surface_Elevation + generator.VoxelType.Surface_DistanceFromSeaLevel
			|| generator.VoxelType.bGenerateReversedSurface &&
			maxZPosition > -generator.VoxelType.Surface_DistanceFromSeaLevel + generator.VoxelType.ReversedSurface_DistanceFromSeaLevel)
		{
			return false;
		}
	}

	return true;
}

int32 UNoiseVoxelGridGenerator::GetVoxelTypeCount() const
{
	return SurfaceGenerators.Num();
}

TTuple<FName, FVoxelType> UNoiseVoxelGridGenerator::GetVoxelTypeById(const int32& voxelTypeIndex) const
{
	auto surfaceGenerator = SurfaceGenerators[voxelTypeIndex];

	return TTuple<FName, FVoxelType>(surfaceGenerator.VoxelName, surfaceGenerator.VoxelType);
}

void UNoiseVoxelGridGenerator::GenerateVoxels(FChunkStruct& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	//This generation is very unoptimized because it is not part of bachelor thesis

	const auto chunkLenght = GetVoxelDimensionCount();
	const int voxelTypeCount = GetVoxelTypeCount();

	const auto gridPos = chunk.GridPosition * chunkLenght;

	// Skip all chunks that dont need to be processed because it is guaranteed they are empty
	if (IsChunkPositionOutOfBounds(gridPos.Z, gridPos.Z + chunkLenght - 1))
	{
		return;
	}

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
					auto surfaceGenerator = SurfaceGenerators[voxelId];

					if (!surfaceGenerator.VoxelType.bShouldGenerate)
					{
						continue;
					}
					
					auto voxel = FVoxel(voxelId, surfaceGenerator.VoxelType.bIsTransparent);

					float posX = x + gridPos.X;
					float posY = y + gridPos.Y;
					double currentElevation = gridPos.Z + z;
					const double elevation = GetSurfaceGradient(posX, posY, surfaceGenerator.SurfaceGenerator, surfaceGenerator.VoxelType);

					bool AddVoxel;

					if (surfaceGenerator.VoxelType.bGenerateReversedSurface)
					{
						const double depth = GetSurfaceGradient(posX, posY, surfaceGenerator.ReverseSurfaceGenerator, surfaceGenerator.VoxelType);
						AddVoxel = currentElevation > -depth && currentElevation < elevation;
					}
					else
					{
						AddVoxel = currentElevation < elevation;
					}

					if (AddVoxel)
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
		auto surfaceGenerator = SurfaceGenerators[voxelId];
		const auto elevation = GetSurfaceGradient(location.X, location.Y, surfaceGenerator.SurfaceGenerator, surfaceGenerator.VoxelType);

		if (elevation > maxElevation)
		{
			maxElevation = elevation;
		}
	}

	return maxElevation * VoxelSize;
}
