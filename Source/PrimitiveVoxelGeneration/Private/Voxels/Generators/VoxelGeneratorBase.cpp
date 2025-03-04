// Fill out your copyright notice in the Description page of Project Settings.

UE_DISABLE_OPTIMIZATION

#include "Voxels/VoxelGenerators/VoxelGeneratorBase.h"

#include "Voxels/VoxelType.h"
#include "Chunks/VoxelIdInChunk.h"
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

void UVoxelGeneratorBase::AddVoxelAtIndex(const TSharedPtr<FChunkStruct>& chunk, const uint32& index,
                                          const FVoxel& voxel)
{
	chunk->Voxels[index] = voxel;
	if (!chunk->ChunkVoxelTypeTable.Contains(voxel.VoxelId))
	{
		chunk->ChunkVoxelTypeTable.Add(voxel.VoxelId, FVoxelIdInChunk(chunk->ChunkVoxelTypeTable.Num(), 1));
	}
	else
	{
		chunk->ChunkVoxelTypeTable.Find(voxel.VoxelId)->CountInChunk++;
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

int32 UVoxelGeneratorBase::GetVoxelTypeCount() const
{
	return VoxelTypeTable->GetRowNames().Num();
}

FVoxel UVoxelGeneratorBase::VoxelTypeToVoxel(const FDataTableRowHandle& rowHandle) const
{
	TArray<FName> RowNames =  VoxelTypeTable->GetRowNames();
	for (int32 Index = 0; Index < RowNames.Num(); Index++)
	{
		if (RowNames[Index] == rowHandle.RowName)
		{
			return FVoxel(Index);
		}
	}

	return FVoxel();
}

FVoxelType UVoxelGeneratorBase::GetVoxelTypeById(const int32& voxelTypeIndex) const
{
	auto rowName = VoxelTypeTable->GetRowNames()[voxelTypeIndex];

	return *VoxelTypeTable->FindRow<FVoxelType>(rowName, "");
}

double UVoxelGeneratorBase::GetHighestElevationAtLocation(const FVector& location)
{
	return GetChunkSize();
}

int32 UVoxelGeneratorBase::GetVoxelIndex(const FIntVector& indexVector) const
{
	return GetVoxelIndex(indexVector.X, indexVector.Y, indexVector.Z);
}

// TODO: optimize, move ChunkVoxelTypeTable to be created during mesh memory initialization
bool UVoxelGeneratorBase::ChangeVoxelIdInChunk(const TSharedPtr<FChunkStruct>& chunk, const FIntVector& voxelPosition,
                                               const FVoxel& voxelId) const
{
	auto index = GetVoxelIndex(voxelPosition);

	if (chunk.IsValid() && chunk->Voxels.IsValidIndex(index))
	{
		if (voxelId.IsEmptyVoxel())
		{
			FVoxel removedVoxel = chunk->Voxels[index];

			if (!removedVoxel.IsEmptyVoxel())
			{
				auto voxelInChunkPtr = chunk->ChunkVoxelTypeTable.Find(removedVoxel.VoxelId);
				voxelInChunkPtr->CountInChunk--;

				if (voxelInChunkPtr->CountInChunk <= 0)
				{
					chunk->ChunkVoxelTypeTable.Remove(removedVoxel.VoxelId);
					if (!chunk->ChunkVoxelTypeTable.IsEmpty())
					{
						int voxelIndex = 0;
						TArray<int32> voxelKeys;
						chunk->ChunkVoxelTypeTable.GetKeys(voxelKeys);
						for (auto key : voxelKeys)
						{
							chunk->ChunkVoxelTypeTable[key].ChunkVoxelId = voxelIndex;
							voxelIndex++;
						}
					}
				}
			}

			chunk->Voxels[index] = voxelId;
		}
		else
		{
			AddVoxelAtIndex(chunk, index, voxelId);
		}
		return true;
	}

	return false;
}
