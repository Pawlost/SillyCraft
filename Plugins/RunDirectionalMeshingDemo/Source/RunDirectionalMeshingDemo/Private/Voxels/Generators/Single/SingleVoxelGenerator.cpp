#include "Voxel/Generators/Single/SingleVoxelGenerator.h"

void USingleVoxelGenerator::GenerateVoxels(FChunkStruct& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	auto voxelFillIndex = GetSingleVoxel();
	auto voxelGridDensity = GetVoxel3DimensionCount();
	
	for (int i = 0; i < voxelGridDensity; i++)
	{
		AddVoxelAtIndex(chunk, i, voxelFillIndex);
	}
}
