// Fill out your copyright notice in the Description page of Project Settings.

#include "Voxels/VoxelGenerators/VoxelGeneratorBase.h"
#include "Meshers/MesherBase.h"

void UVoxelGeneratorBase::BeginPlay()
{
	VoxelCountY = VoxelDimensionCount;
	ChunkSize = VoxelCountY * VoxelSize;
	VoxelCountYZ = VoxelCountY * VoxelCountY;
	VoxelCountXYZ = VoxelCountYZ * VoxelCountY;

	Super::BeginPlay();

	checkf(MesherBlueprint, TEXT("Mesher blueprint must be set"));
	if (MesherBlueprint)
	{
		// Create the component dynamically
		Mesher = NewObject<UMesherBase>(this, MesherBlueprint);

		if (Mesher)
		{
			Mesher->SetVoxelGenerator(this);
			Mesher->RegisterComponent();
		}
	}
}

void UVoxelGeneratorBase::AddVoxelAtIndex(FChunkStruct& chunk, const uint32& index,
                                          const FVoxel& voxel)
{
	if (!IsValid(this))
	{
		return;
	}

	FScopeLock Lock(&Mutex);
	auto prevVoxel = chunk.Voxels[index];
	if (!prevVoxel.IsEmptyVoxel())
	{
		(*chunk.ChunkVoxelTypeTable.Find(prevVoxel.VoxelId))--;
	}

	chunk.Voxels[index] = voxel;
	if (chunk.ChunkVoxelTypeTable.Contains(voxel.VoxelId))
	{
		(*chunk.ChunkVoxelTypeTable.Find(voxel.VoxelId))++;
	}
	else
	{
		chunk.ChunkVoxelTypeTable.Add(voxel.VoxelId, 1);
	}
}

int32 UVoxelGeneratorBase::GetVoxelIndex(const int32 x, const int32 y, const int32 z) const
{
	return y + (z * VoxelCountY) + (x * VoxelCountYZ);
}

double UVoxelGeneratorBase::GetChunkSize()
{
	return ChunkSize;
}

int32 UVoxelGeneratorBase::GetVoxelDimensionCount() const
{
	return VoxelDimensionCount;
}

double_t UVoxelGeneratorBase::GetVoxelSize()
{
	return VoxelSize;
}

int32 UVoxelGeneratorBase::GetVoxel2DimensionCount() const
{
	return VoxelCountYZ;
}

int32 UVoxelGeneratorBase::GetVoxel3DimensionCount() const
{
	return VoxelCountXYZ;
}

void UVoxelGeneratorBase::GenerateMesh(FChunkFaceParams& faceParams) const
{
	Mesher->GenerateMesh(faceParams);
}

double UVoxelGeneratorBase::GetHighestElevationAtLocation(const FVector& location)
{
	return GetChunkSize();
}

int32 UVoxelGeneratorBase::GetVoxelIndex(const FIntVector& indexVector) const
{
	return GetVoxelIndex(indexVector.X, indexVector.Y, indexVector.Z);
}

bool UVoxelGeneratorBase::ChangeVoxelIdInChunk(const TSharedPtr<FChunkStruct>& chunk, const FIntVector& voxelPosition,
                                               const FVoxel& voxelId)
{
	auto index = GetVoxelIndex(voxelPosition);

	if (chunk.IsValid() && chunk->Voxels.IsValidIndex(index))
	{
		if (voxelId.IsEmptyVoxel())
		{
			FVoxel removedVoxel = chunk->Voxels[index];

			if (!removedVoxel.IsEmptyVoxel())
			{
				auto& voxelCount = *chunk->ChunkVoxelTypeTable.Find(removedVoxel.VoxelId);
				voxelCount--;

				if (voxelCount <= 0)
				{
					chunk->ChunkVoxelTypeTable.Remove(removedVoxel.VoxelId);
				}
			}

			chunk->Voxels[index] = voxelId;
		}
		else
		{
			AddVoxelAtIndex(*chunk, index, voxelId);
		}
		return true;
	}

	return false;
}
