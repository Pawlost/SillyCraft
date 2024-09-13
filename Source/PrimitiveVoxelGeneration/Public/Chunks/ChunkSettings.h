// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ChunkSettings.generated.h"

/**
 * 
 */


USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FUChunkSettings
{
	GENERATED_BODY()
	
public:
	FDataTableRowHandle GetVoxelTypes() const;
	void SetVoxelTypes(const FDataTableRowHandle& VoxelTypes);
	int64 GetSeed() const;
	void SetSeed(const int64 Seed);
	int32 GetChunkWidthInBlocks() const;
	void SetChunkWidthInBlocks(const int32 ChunkWidthInBlocks);
	int32 GetVoxelSize() const;
	void SetVoxelSize(const int32 VoxelSize);

protected:
	UPROPERTY()
	FDataTableRowHandle VoxelTypes;

	UPROPERTY()
	int64 Seed = 0;

	UPROPERTY()
	int32 ChunkWidthInBlocks = 0;

	UPROPERTY()
	int32 VoxelSize = 0;
};
