#include "Voxel/Generators/Single/FractionFillVoxelGridGenerator.h"

void UFractionFillVoxelGridGenerator::GenerateVoxels(FChunk& Chunk)
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto ChunkDimension = GetVoxelCountPerChunkDimension();

	for (int x = 0; x < ChunkDimension / XFraction; x++)
	{
		for (int y = 0; y < ChunkDimension / YFraction; y++)
		{
			for (int z = 0; z < ChunkDimension / ZFraction; z++)
			{
				const auto Index = CalculateVoxelIndex(x, y, z);
				ChangeKnownVoxelAtIndex(Chunk, Index, VoxelFillIndex);
			}
		}
	}
}
