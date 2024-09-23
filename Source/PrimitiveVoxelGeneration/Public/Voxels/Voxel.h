﻿#pragma once

#include "CoreMinimal.h"
#include "Voxel.generated.h"

// Should not be positive
#define EMPTY_VOXEL -1

USTRUCT()
struct FVoxel
{
	GENERATED_BODY()
	
	// Initial voxel id
	int32 VoxelId = EMPTY_VOXEL;

	bool IsEmptyVoxel() const
	{
		return VoxelId == EMPTY_VOXEL;
	}
};