#include "Voxel/Generators/Single/FractionFillVoxelGridGenerator.h"

void UFractionFillVoxelGridGenerator::GenerateVoxels(FChunk& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	auto voxelFillIndex = GetSingleVoxel();
	const auto chunkLenght = GetVoxelCountPerChunkDimension();
		
	for (int x = 0; x < chunkLenght/XFraction; x++)
	{
		for (int y = 0; y < chunkLenght/YFraction; y++)
		{
			for (int z = 0; z < chunkLenght/ZFraction; z++)
			{
				const auto index = CalculateVoxelIndex(x, y, z);

				ChangeKnownVoxelAtIndex(chunk, index, voxelFillIndex);
			}
		}
	}
}
