// Fill out your copyright notice in the Description page of Project Settings.
#include "Voxels/VoxelGenerators/FractionFillVoxelGridGenerator.h"

void UFractionFillVoxelGridGenerator::GenerateVoxels(TSharedPtr<FChunkStruct>& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	auto voxelFillIndex = VoxelTypeToVoxel(RowHandle);
	const auto chunkLenght = GetVoxelDimensionCount();
		
	for (int x = 0; x < chunkLenght/XFraction; x++)
	{
		for (int y = 0; y < chunkLenght/YFraction; y++)
		{
			for (int z = 0; z < chunkLenght/ZFraction; z++)
			{
				const auto index = GetVoxelIndex(x, y, z);

				AddVoxelAtIndex(chunk, index, voxelFillIndex);
			}
		}
	}
}
