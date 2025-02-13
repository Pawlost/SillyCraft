// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/VoxelGridGenerator.h"

#include "FastNoiseWrapper.h"
#include "Voxels/ChunkStruct.h"
#include "Voxels/Voxel.h"
#include "Voxels/VoxelType.h"

void UVoxelGridGenerator::GenerateVoxels(FChunkStruct& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif
	
	auto chunkLenght = ChunkSettings->GetAxisVoxelCount();
	auto maxElevation = ChunkSettings->GetMaximumElevation();

	auto gridPos = ChunkGridPos * chunkLenght;
	int voxelIds = ChunkGridData->GetVoxelIdCount();
	chunk.VoxelIdTable.Reserve(voxelIds);
	
	for(int voxelId = 0; voxelId < voxelIds; voxelId++)
	{
		FVoxelType voxelType = ChunkGridData->GetVoxelTypeById(voxelId);
		
		auto voxel = FVoxel(voxelId);
		
		Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, voxelType.Seed,  voxelType.NoiseFrequency);
		
		for (int x = 0; x < chunkLenght; x++) 
		{
			for (int y = 0; y < chunkLenght; y++) 
			{
				auto elevation = Noise->GetNoise2D(x + gridPos.X,  y + gridPos.Y) * maxElevation;
				
				for (int z = 0; z < chunkLenght; z++) 
				{
					auto index = ChunkSettings->GetVoxelIndex(x, y, z);
					
					if (gridPos.Z + z <= elevation) 
					{
						chunk.Voxels[index] = voxel;
						if(!chunk.VoxelIdTable.Contains(voxel.VoxelId))
						{
							chunk.VoxelIdTable.Add(voxel.VoxelId, chunk.VoxelIdTable.Num());
						}
						Empty = false;
					}
				}
			}
		}
	}
}