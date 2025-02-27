// Fill out your copyright notice in the Description page of Project Settings.
#include "Voxels/VoxelGenerators/SingleVoxelGenerator.h"
#include "Voxels/Voxel.h"

void USingleVoxelGenerator::GenerateVoxels(TSharedPtr<FChunkStruct>& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	const int voxelTypeCount = GetVoxelTypeCount();
	chunk->ChunkVoxelTypeTable.Reserve(voxelTypeCount);
	chunk->Voxels.SetNum(GetVoxel3DimensionCount());
	uint32 index = 0;
	for (int voxelId = 0; voxelId < voxelTypeCount; voxelId++)
	{
		AddVoxelAtIndex(chunk, index, FVoxel(voxelId));
		index ++;
	}
}
