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
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntVector FractionOfVoxelGridDimension;
	
	virtual void GenerateVoxels(TSharedPtr<FChunkStruct>& chunk) override;
};
