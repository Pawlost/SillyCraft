#pragma once

#include "CoreMinimal.h"
#include "Voxel.generated.h"

// Should not be positive
#define EMPTY_VOXEL -1

USTRUCT(BlueprintType)
struct FVoxel
{
	GENERATED_BODY()

	// Initial voxel id
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 VoxelId = EMPTY_VOXEL;

	bool IsEmptyVoxel() const
	{
		return VoxelId == EMPTY_VOXEL;
	}
};


inline bool operator==(const FVoxel& Voxel, const FVoxel& OtherVoxel)
{
	return Voxel.VoxelId == OtherVoxel.VoxelId;
}

inline bool operator!=(const FVoxel& Voxel, const FVoxel& OtherVoxel)
{
	return Voxel.VoxelId != OtherVoxel.VoxelId;
}
