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
		
		auto noiseGenerator = FSurfaceGenerator();
		noiseGenerator.GenerateReverseSurface = voxelType.Value.GenerateReversedSurface;
		noiseGenerator.IsTransparent = voxelType.Value.IsTransparent;

		noiseGenerator.SurfaceGenerator.SurfaceNoise = surfaceNoise;
		// Value is divided by voxel size so the real world elevation reaches exact mark in UE world coordinated
		noiseGenerator.SurfaceGenerator.MaxElevation = voxelType.Value.SurfaceElevation / GetVoxelSize();
		noiseGenerator.SurfaceGenerator.RangeFromSeaLevel = voxelType.Value.RangeFromSeaLevel / GetVoxelSize();

		noiseGenerator.ReverseSurfaceGenerator.SurfaceNoise = reversedSurfaceNoise;
		noiseGenerator.ReverseSurfaceGenerator.MaxElevation = voxelType.Value.ReversedSurfaceDepth / GetVoxelSize();
		noiseGenerator.ReverseSurfaceGenerator.RangeFromSeaLevel = voxelType.Value.ReversedRangeFromSeaLevel / GetVoxelSize();
		
		SurfaceGenerators.Add(noiseGenerator);
	}
}

double UNoiseVoxelGridGenerator::GetSurfaceGradient(float posX, float posY, const FNoiseSurfaceGenerator& generator)
{
	return generator.SurfaceNoise->GetNoise2D(posX, posY) * generator.MaxElevation + generator.RangeFromSeaLevel;
}

bool UNoiseVoxelGridGenerator::IsChunkPositionOutOfBounds(double minZPosition, double maxZPosition)
{
	for (auto generator : SurfaceGenerators)
	{
		if (minZPosition < generator.SurfaceGenerator.MaxElevation + generator.SurfaceGenerator.RangeFromSeaLevel
			|| generator.GenerateReverseSurface &&  
			maxZPosition > -static_cast<double>(generator.ReverseSurfaceGenerator.MaxElevation + generator.ReverseSurfaceGenerator.
				RangeFromSeaLevel)){
			return false;	
		}
	}

	return true;
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
					
					auto voxel = FVoxel(voxelId, surfaceGenerator.IsTransparent);
					
					float posX = x + gridPos.X;
					float posY =  y + gridPos.Y;
					double currentElevation = gridPos.Z + z;
					const double elevation = GetSurfaceGradient(posX, posY, surfaceGenerator.SurfaceGenerator);
					
					bool AddVoxel;
					
					if (surfaceGenerator.GenerateReverseSurface)
					{
						const double depth = GetSurfaceGradient(posX, posY, surfaceGenerator.ReverseSurfaceGenerator);
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
		auto surfaceGenerator = SurfaceGenerators[voxelId];
		const auto elevation = GetSurfaceGradient(location.X, location.Y, surfaceGenerator.SurfaceGenerator);

		if (elevation > maxElevation)
		{
			maxElevation = elevation;
		}
	}

	return maxElevation * VoxelSize;
}
