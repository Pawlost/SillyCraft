#pragma once

#include "CoreMinimal.h"
#include "Voxel.generated.h"

/**
 * Represents a single voxel in the voxel grid.
 * Contains an ID and a transparency flag.
 */
USTRUCT()
struct RUNDIRECTIONALMESHINGDEMO_API FVoxel
{
	GENERATED_BODY()

	// Tied to FVoxel namespace
	static const int32 EMPTY_VOXEL = -1; 

	// Saving voxel grids is not implemented in this demo, but a property specifier for saving has been added.
	UPROPERTY(SaveGame)
	int32 VoxelId = EMPTY_VOXEL;
	
	UPROPERTY(SaveGame)
	bool bIsTransparent = false;
	
	FORCEINLINE bool IsEmptyVoxel() const
	{
		return VoxelId == EMPTY_VOXEL;
	}

	FORCEINLINE bool IsTransparent() const
	{
		return bIsTransparent || IsEmptyVoxel();
	}

	FORCEINLINE bool operator==(const FVoxel& OtherVoxel) const
	{
		// Transparent is not considered because VoxelIds are the same.
		return VoxelId == OtherVoxel.VoxelId;
	}

	FORCEINLINE bool operator!=(const FVoxel& OtherVoxel) const
	{
		return VoxelId != OtherVoxel.VoxelId;
	}
};
