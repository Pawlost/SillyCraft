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

		 auto voxelType = GetVoxelTypeById(voxelId);
		surfaceNoise->SetupFastNoise(voxelType.Value.SurfaceNoiseType, voxelType.Value.SurfaceSeed, voxelType.Value.SurfaceNoiseFrequency);
		reversedSurfaceNoise->SetupFastNoise(voxelType.Value.ReversedSurfaceNoiseType, voxelType.Value.ReversedSurfaceSeed, voxelType.Value.ReversedSurfaceNoiseFrequency);
		
		auto noiseGenerator = FNoiseGeneratorVariables(surfaceNoise, reversedSurfaceNoise, voxelType.Value.SurfaceElevation / GetVoxelSize(), voxelType.Value.ReversedSurfaceDepth / GetVoxelSize(), voxelType.Value.GenerateReversedSurface, voxelType.Value.IsTransparent);
		NoiseGenerators.Add(noiseGenerator);
	}
}

int32 UNoiseVoxelGridGenerator::GetVoxelTypeCount() const
{
	return VoxelTypeTable->GetRowNames().Num();
}

TTuple<FName, FVoxelType> UNoiseVoxelGridGenerator::GetVoxelTypeById(const int32& voxelTypeIndex) const
{
	auto rowName = VoxelTypeTable->GetRowNames()[voxelTypeIndex];

	return TTuple<FName, FVoxelType>(rowName, *VoxelTypeTable->FindRow<FVoxelType>(rowName, ""));
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
					auto noiseVariables = NoiseGenerators[voxelId];
					
					auto voxel = FVoxel(voxelId, noiseVariables.IsTransparent);

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
