﻿// Fill out your copyright notice in the Description page of Project Settings.

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
	
	int32 GetVoxelIndex(const int32 x, const int32 y, const int32 z) const;
	FIntVector3 IndexToCoords(const int32 index) const;
	
	// Getters & Setters
	FDataTableRowHandle GetVoxelTypes() const;
	void SetVoxelTypes(const FDataTableRowHandle& VoxelTypes);
	int64 GetSeed() const;
	void SetSeed(const int64 Seed);
	int32 GetChunkSideSizeInVoxels() const;
	void SetChunkSizeInVoxels(const int32 chunkSideSizeInVoxels);
	int32 GetVoxelSize() const;
	void SetVoxelSize(const int32 VoxelSize);
	int32 GetMaximumElevation() const;
	void SetMaximumElevation(const int32 MaximumElevation);
	int32 GetChunkPlaneSizeInVoxels() const;
	double GetNoiseFrequency() const;
	void SetNoiseFrequency(const double NoiseFrequency);
	
private:
	UPROPERTY()
	FDataTableRowHandle VoxelTypes;

	UPROPERTY()
	int64 Seed = 1234;

	UPROPERTY()
	int32 VoxelSize = 10;

	UPROPERTY()
	double MaximumElevation = 200;

	UPROPERTY()
	int32 ChunkSideSizeInVoxels = 10;

	UPROPERTY()
	int32 ChunkPlaneSizeInVoxels = 100;

	UPROPERTY()
	double NoiseFrequency = 0.01;
};