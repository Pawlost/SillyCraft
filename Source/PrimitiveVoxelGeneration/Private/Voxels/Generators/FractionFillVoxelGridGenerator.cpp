// Fill out your copyright notice in the Description page of Project Settings.
#include "Voxels/VoxelGenerators/FractionFillVoxelGridGenerator.h"

void UFractionFillVoxelGridGenerator::GenerateVoxels(TSharedPtr<FChunkStruct>& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	const int voxelTypeCount = GetVoxelTypeCount();
	chunk->ChunkVoxelTypeTable.Reserve(voxelTypeCount);
	chunk->Voxels.SetNum(GetVoxel3DimensionCount());

	auto voxelFillIndex = VoxelTypeToVoxel(RowHandle);
	const auto chunkLenght = GetVoxelDimensionCount();
		
	for (int x = 0; x < chunkLenght/FractionOfVoxelGridDimension.X; x++)
	{
		for (int y = 0; y < chunkLenght/FractionOfVoxelGridDimension.Y; y++)
		{
			for (int z = 0; z < chunkLenght/FractionOfVoxelGridDimension.Z; z++)
			{
				const auto index = GetVoxelIndex(x, y, z);

				AddVoxelAtIndex(chunk, index, voxelFillIndex);
			}
		}
	}
}
