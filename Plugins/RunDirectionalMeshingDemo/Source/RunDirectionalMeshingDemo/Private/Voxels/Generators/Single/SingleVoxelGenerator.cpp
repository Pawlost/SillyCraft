#include "Voxel/Generators/Single/SingleVoxelGenerator.h"

void USingleVoxelGenerator::GenerateVoxels(FChunk& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	auto voxelFillIndex = GetSingleVoxel();
	auto voxelGridDensity = GetVoxelCountPerChunk();
	
	for (int i = 0; i < voxelGridDensity; i++)
	{
		ChangeKnownVoxelAtIndex(chunk, i, voxelFillIndex);
	}
}
