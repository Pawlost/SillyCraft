// Fill out your copyright notice in the Description page of Project Settings.
#include "Voxel/Generators/VoxelGeneratorBase.h"

#include "Chunk/Chunk.h"
#include "Mesher/MesherBase.h"

void UVoxelGeneratorBase::BeginPlay()
{
	// Calculate the total number of voxels in a chunk along each axis
	VoxelCountY = VoxelCountPerChunkDimension;
	ChunkSize = VoxelCountY * VoxelSize;
	VoxelCountYZ = VoxelCountY * VoxelCountY;
	VoxelCountXYZ = VoxelCountYZ * VoxelCountY;

	Super::BeginPlay();

	checkf(MesherBlueprint, TEXT("Mesher blueprint must be set"));
	if (MesherBlueprint)
	{
		// Register mesher
		Mesher = NewObject<UMesherBase>(this, MesherBlueprint);

		if (Mesher)
		{
			Mesher->SetVoxelGenerator(this);
			Mesher->RegisterComponent();
		}
	}
}

void UVoxelGeneratorBase::ChangeKnownVoxelAtIndex(FChunk& Chunk, const uint32& Index,
                                                  const FVoxel& Voxel)
{
	// NOTICE: Code here is optimized because voxel grid generation is not main topic of this bachelor's thesis 
	FScopeLock Lock(&Mutex);
	const auto PrevVoxel = Chunk.VoxelGrid[Index];
	RemoveVoxelFromChunkTable(Chunk, PrevVoxel);

	// Replace the voxel
	Chunk.VoxelGrid[Index] = Voxel;

	const auto VoxelTypeAmountInChunk = Chunk.ChunkVoxelIdTable.Find(Voxel.VoxelId);
	if (VoxelTypeAmountInChunk != nullptr)
	{
		//Increase amount of added voxel
		(*VoxelTypeAmountInChunk) += 1;
	}
	else
	{
		//If voxel is unique, start new count
		Chunk.ChunkVoxelIdTable.Add(Voxel.VoxelId, 1);
	}
}

bool UVoxelGeneratorBase::ChangeUnknownVoxelIdInChunk(const TSharedPtr<FChunk>& Chunk, const FIntVector& VoxelPosition,
                                                      const FName& VoxelName)
{
	const auto Index = CalculateVoxelIndex(VoxelPosition);
	const auto VoxelId = GetVoxelByName(VoxelName);

	// Check if chunk and position is valid.
	if (Chunk.IsValid() && Chunk->VoxelGrid.IsValidIndex(Index))
	{
		// Default unknown voxels are empty
		if (VoxelId.IsEmptyVoxel())
		{
			const FVoxel RemovedVoxel = Chunk->VoxelGrid[Index];
			RemoveVoxelFromChunkTable(*Chunk, RemovedVoxel);

			// Make previous voxel position empty.
			Chunk->VoxelGrid[Index] = VoxelId;
		}
		else
		{
			// If voxel is known we get specific Id
			ChangeKnownVoxelAtIndex(*Chunk, Index, VoxelId);
		}
		return true;
	}

	return false;
}

uint32 UVoxelGeneratorBase::CalculateVoxelIndex(const int32 X, const int32 Y, const int32 Z) const
{
	return Y + (Z * VoxelCountY) + (X * VoxelCountYZ);
}

uint32 UVoxelGeneratorBase::CalculateVoxelIndex(const FIntVector& VoxelPosition) const
{
	return CalculateVoxelIndex(VoxelPosition.X, VoxelPosition.Y, VoxelPosition.Z);
}

double UVoxelGeneratorBase::GetChunkAxisSize() const
{
	return ChunkSize;
}

double UVoxelGeneratorBase::GetVoxelSize() const
{
	return VoxelSize;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerChunkDimension() const
{
	return VoxelCountPerChunkDimension;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerChunkLayer() const
{
	return VoxelCountYZ;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerChunk() const
{
	return VoxelCountXYZ;
}

void UVoxelGeneratorBase::GenerateMesh(FMesherVariables& MesherVariables) const
{
	Mesher->GenerateMesh(MesherVariables);
}

double UVoxelGeneratorBase::GetHighestElevationAtLocation(const FVector& Location)
{
	return GetChunkAxisSize();
}

void UVoxelGeneratorBase::RemoveVoxelFromChunkTable(FChunk& Chunk, const FVoxel& Voxel)
{
	const auto VoxelTypeAmountInChunk = Chunk.ChunkVoxelIdTable.Find(Voxel.VoxelId);
	if (VoxelTypeAmountInChunk != nullptr && !Voxel.IsEmptyVoxel())
	{
		auto& VoxelTypeAmountInChunkRef = *VoxelTypeAmountInChunk;
		// Reduce amount of previous voxel type because it will be replaced.
		VoxelTypeAmountInChunkRef--;

		// Amount is initialized with 1
		if (VoxelTypeAmountInChunkRef < 1)
		{
			// Remove id from a table if there is no voxel representation
			Chunk.ChunkVoxelIdTable.Remove(Voxel.VoxelId);
		}
	}
}
