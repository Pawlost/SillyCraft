// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkBase.h"
#include "FastNoiseWrapper.h"
#include "DefaultChunk.generated.h"

struct FVoxelFace;
/**
 * 
 */
UCLASS()
class PRIMITIVEVOXELGENERATION_API UDefaultChunk : public UChunkBase
{
	GENERATED_BODY()

public:
	UDefaultChunk();
	void NaiveMeshing(int voxelChunk, int prevIndexDifference, int32 frontIndex,
		FVoxelFace& face, TArray<FVoxelFace>& faces);
	
	virtual void GenerateVoxels(FVector& origin) override;
	virtual void GenerateMesh(UProceduralMeshComponent* procMesh) override;
	virtual void SetChunkSettings(const TSharedPtr<FGenerationSettings> ChunkSettings) override;
	virtual int32 VoxelAt(int32 index) override;

private:
	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> Noise;
	TArray<int32> Voxels;
};
