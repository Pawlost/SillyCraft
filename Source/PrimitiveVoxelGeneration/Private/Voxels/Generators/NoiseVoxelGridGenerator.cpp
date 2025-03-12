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
		TObjectPtr<UFastNoiseWrapper> surfaceNoise = NewObject<UFastNoiseWrapper>(this);
		TObjectPtr<UFastNoiseWrapper> reversedSurfaceNoise = NewObject<UFastNoiseWrapper>(this);

		const FVoxelType voxelType = GetVoxelTypeById(voxelId);
		surfaceNoise->SetupFastNoise(voxelType.SurfaceNoiseType, voxelType.SurfaceSeed, voxelType.SurfaceNoiseFrequency);
		reversedSurfaceNoise->SetupFastNoise(voxelType.ReversedSurfaceNoiseType, voxelType.ReversedSurfaceSeed, voxelType.ReversedSurfaceNoiseFrequency);
		
		auto noiseGenerator = FNoiseGeneratorVariables(surfaceNoise, reversedSurfaceNoise, voxelType.SurfaceElevation / GetVoxelSize(), voxelType.ReversedSurfaceDepth / GetVoxelSize(), voxelType.GenerateReversedSurface);
		NoiseGenerators.Add(noiseGenerator);
	}
}


void UNoiseVoxelGridGenerator::GenerateVoxels(FChunkStruct& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	const auto chunkLenght = GetVoxelDimensionCount();
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

					auto noiseVariables = NoiseGenerators[voxelId];

					const double elevation = noiseVariables.SurfaceGenerator->GetNoise2D(x + gridPos.X, y + gridPos.Y) * noiseVariables.MaxElevation;
					const double depth = noiseVariables.ReverseSurfaceGenerator->GetNoise2D(x + gridPos.X, y + gridPos.Y) * noiseVariables.MaxDepth;

					bool AddVoxel;
					double currentElevation = gridPos.Z + z;
					
					if (noiseVariables.GenerateReverseSurface)
					{
						AddVoxel = currentElevation > -depth  && currentElevation < elevation;
					}else
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
		auto noiseVariables = NoiseGenerators[voxelId];
		const auto elevation = noiseVariables.SurfaceGenerator->GetNoise2D(location.X, location.Y) * noiseVariables.MaxElevation;

		if (elevation > maxElevation)
		{
			maxElevation = elevation;
		}
	}

	return maxElevation * VoxelSize;
}
