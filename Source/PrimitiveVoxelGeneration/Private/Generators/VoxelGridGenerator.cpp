// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/VoxelGridGenerator.h"

#include "FastNoiseWrapper.h"
#include "Voxels/ChunkStruct.h"
#include "Voxels/Voxel.h"
#include "Voxels/VoxelType.h"

void UVoxelGridGenerator::GenerateVoxels(TSharedPtr<FChunkStruct>& chunk)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation")
#endif

	const auto chunkLenght = GetVoxelDimensionCount();

	const auto gridPos = chunk->GridPosition * chunkLenght;
	const int voxelTypeCount = GetVoxelTypeCount();
	chunk->ChunkVoxelTypeTable.Reserve(voxelTypeCount);
	chunk->Voxels.SetNum(GetVoxel3DimensionCount());

	for (int voxelId = 0; voxelId < voxelTypeCount; voxelId++)
	{
		const FVoxelType voxelType = GetVoxelTypeById(voxelId);

		auto voxel = FVoxel(voxelId);

		Noise->SetupFastNoise(EFastNoise_NoiseType::ValueFractal, voxelType.Seed, voxelType.NoiseFrequency);

		for (int x = 0; x < chunkLenght; x++)
		{
			for (int y = 0; y < chunkLenght; y++)
			{
				const auto elevation = Noise->GetNoise2D(x + gridPos.X, y + gridPos.Y) * GetMaximumElevation();

				for (int z = 0; z < chunkLenght; z++)
				{
					const auto index = GetVoxelIndex(x, y, z);

					if (gridPos.Z + z <= elevation)
					{
						chunk->Voxels[index] = voxel;
						if (!chunk->ChunkVoxelTypeTable.Contains(voxel.VoxelId))
						{
							chunk->ChunkVoxelTypeTable.Add(voxel.VoxelId, chunk->ChunkVoxelTypeTable.Num());
						}
						chunk->IsEmpty = false;
					}
				}
			}
		}
	}
}
