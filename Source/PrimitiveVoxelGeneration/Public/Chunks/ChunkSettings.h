// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ChunkSettings.generated.h"

class AChunkActor;
/**
 * 
 */
USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkSettings
{
	GENERATED_BODY()
	
	int32 GetChunkSize() const;
	
	int32 GetVoxelIndex(const int32 x, const int32 y, const int32 z) const;
	FIntVector3 IndexToCoords(const int32 index) const;
	
	// Getters & Setters
	int32 GetChunkSideSizeInVoxels() const;
	void SetChunkSizeInVoxels(const int32 chunkSideSizeInVoxels);
	double GetVoxelSize() const;
	void SetVoxelSize(const double VoxelSize);
	double GetMaximumElevation() const;
	void SetMaximumElevation(const double MaximumElevation);
	int32 GetChunkPlaneSizeInVoxels() const;
	
private:
	UPROPERTY()
	int32 VoxelSize = 10;

	UPROPERTY()
	double MaximumElevation = 200;

	UPROPERTY()
	int32 ChunkSideSizeInVoxels = 10;

	UPROPERTY()
	int32 ChunkPlaneSizeInVoxels = 100;
};