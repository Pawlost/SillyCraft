#include "Voxel/Generators/Single/SingleVoxelGenerator.h"

#include "Chunk/Chunk.h"

void USingleVoxelGenerator::GenerateVoxels(FChunk& Chunk)
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto VoxelGridDensity = GetVoxelCountPerChunk();

	Chunk.VoxelGrid.Init(VoxelFillIndex, VoxelGridDensity);
	Chunk.ChunkVoxelIdTable.Add(VoxelFillIndex.VoxelId, VoxelGridDensity);
}
