// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "SingleVoxelGeneratorBase.h"
#include "Chunks/ChunkStruct.h"
#include "FractionFillVoxelGridGenerator.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class PRIMITIVEVOXELGENERATION_API UFractionFillVoxelGridGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"), Category = "Voxel grid size")
	int32 XFraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"), Category = "Voxel grid size")
	int32 YFraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"), Category = "Voxel grid size")
	int32 ZFraction;
	
	virtual void GenerateVoxels(FChunkStruct& chunk) override;
};
