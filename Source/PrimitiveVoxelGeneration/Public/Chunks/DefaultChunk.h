// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkBase.h"
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
	virtual void SetChunkSettings(const TSharedPtr<FChunkSettings> ChunkSettings) override;
	virtual int32 VoxelAt(int32 index) override;

private:
	void AddNaiveMeshedFace(int32 frontVoxelPos,
		FChunkFace& face, TArray<FChunkFace>& faces);

	bool CheckBorderMin(int min, int32 forwardChunkIndex);
	bool CheckBorderMax(int max, int32 forwardChunkIndex);
	
	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> Noise;
	TArray<int32> Voxels;
};
