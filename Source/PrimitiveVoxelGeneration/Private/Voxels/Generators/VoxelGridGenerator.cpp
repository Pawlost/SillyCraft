// Fill out your copyright notice in the Description page of Project Settings.
#include "Voxels/VoxelGenerators/VoxelGridGenerator.h"

void UVoxelGridGenerator::GenerateVoxels(TSharedPtr<FChunkStruct>& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	auto voxelFillIndex = VoxelTypeToVoxel(RowHandle);
	
	const int voxelTypeCount = GetVoxelTypeCount();
	chunk->ChunkVoxelTypeTable.Reserve(voxelTypeCount);
	auto voxelGridDensity = GetVoxel3DimensionCount();
	chunk->Voxels.SetNum(voxelGridDensity);
	for (int i = 0; i < voxelGridDensity; i++)
	{
		AddVoxelAtIndex(chunk, i, voxelFillIndex);
	}
}
