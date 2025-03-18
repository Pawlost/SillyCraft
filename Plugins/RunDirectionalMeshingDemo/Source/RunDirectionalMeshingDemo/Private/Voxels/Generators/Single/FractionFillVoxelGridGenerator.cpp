#include "Voxel/Generators/Single/FractionFillVoxelGridGenerator.h"

void UFractionFillVoxelGridGenerator::GenerateVoxels(FChunk& Chunk)
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto ChunkDimension = GetVoxelCountPerChunkDimension();

	for (uint32 x = 0; x < ChunkDimension / XFraction; x++)
	{
		for (uint32 y = 0; y < ChunkDimension / YFraction; y++)
		{
			for (uint32 z = 0; z < ChunkDimension / ZFraction; z++)
			{
				const auto Index = CalculateVoxelIndex(x, y, z);
				ChangeKnownVoxelAtIndex(Chunk, Index, VoxelFillIndex);
			}
		}
	}
}
