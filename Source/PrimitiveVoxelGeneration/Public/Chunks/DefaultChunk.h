﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkBase.h"
#include "ChunkGridData.h"
#include "FastNoiseWrapper.h"
#include "DefaultChunk.generated.h"

struct FVoxel;
struct FChunkFace;
/**
 * 
 */
UCLASS()
class PRIMITIVEVOXELGENERATION_API UDefaultChunk : public UChunkBase
{
public:
	virtual void AddToGrid(const TWeakObjectPtr<UChunkGridData> chunkGridData, FIntVector& chunkGridPos) override;

private:
	GENERATED_BODY()

public:
	UDefaultChunk();
	virtual void GenerateVoxels() override;
	virtual void GenerateMesh() override;
	virtual FVoxel VoxelAt(int32 index) override;

private:
	void AddNaiveMeshedFace(FChunkFace& face, TArray<FChunkFace>& faces);

	bool ChunkCull(int32 chunkIndex, FIntVector& neighborChunkDistance) const;
	bool VoxelCull(int32 forwardVoxelIndex);

	bool CrossChunkCullMin(int min, int32 forwardVoxelIndex, int32 chunkIndex, FIntVector neighborChunkDistance);
	bool CrossChunkCullMax(int max, int32 forwardVoxelIndex, int32 chunkIndex, FIntVector neighborChunkDistance);
	
	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> Noise;
	TArray<FVoxel> Voxels;

	TSharedPtr<FChunkSettings> ChunkSettings;
};
