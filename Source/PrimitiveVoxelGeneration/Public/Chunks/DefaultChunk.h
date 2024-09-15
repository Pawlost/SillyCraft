// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkBase.h"
#include "ChunkGridData.h"
#include "FastNoiseWrapper.h"
#include "DefaultChunk.generated.h"

struct FChunkFace;
/**
 * 
 */
UCLASS()
class PRIMITIVEVOXELGENERATION_API UDefaultChunk : public UChunkBase
{
	GENERATED_BODY()

public:
	UDefaultChunk();
	virtual void GenerateVoxels(FIntVector& chunkGridPos) override;
	virtual void GenerateMesh(UProceduralMeshComponent* procMesh, FIntVector& chunkGridPos) override;
	virtual void SetChunkGridData(const TSharedPtr<FChunkGridData> chunkGridData) override;
	virtual int32 VoxelAt(int32 index) override;

private:
	void AddNaiveMeshedFace(FChunkFace& face, TArray<FChunkFace>& faces);

	bool ChunkCull(int32 chunkIndex, FIntVector& neigborChunkCoords);
	bool VoxelCull(int32 forwardVoxelIndex);

	bool CrossChunkCullMin(int min, int32 forwardVoxelIndex, int32 chunkIndex, FIntVector& neighborChunkCoords);
	bool CrossChunkCullMax(int max, int32 forwardVoxelIndex, int32 chunkIndex, FIntVector& neighborChunkCoords);
	
	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> Noise;
	TArray<int32> Voxels;

	TSharedPtr<FChunkGridData> ChunkGridData;
	TSharedPtr<FChunkSettings> ChunkSettings;
};
