// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "VoxelGridGenerator.h"
#include "Chunks/ChunkStruct.h"
#include "FractionFillVoxelGridGenerator.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class PRIMITIVEVOXELGENERATION_API UFractionFillVoxelGridGenerator : public UVoxelGridGenerator
{
	GENERATED_BODY()

public:

	//TODO: rewrite meta
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"), Category = "Grid position")
	int32 XFraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"), Category = "Grid position")
	int32 YFraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"), Category = "Grid position")
	int32 ZFraction;
	
	virtual void GenerateVoxels(TSharedPtr<FChunkStruct>& chunk) override;
};
