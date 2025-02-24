// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxelType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EHardness : uint8
{
	Empty, Soft, Medium, Hard
};

USTRUCT(BlueprintType)
struct PRIMITIVEVOXELGENERATION_API FVoxelType : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Basic")
	FName BlockName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Basic")
	EHardness BlockHardness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Basic")
	int32 Range;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Basic")
	TObjectPtr<UMaterial> Material;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Noise")
	int64 Seed = 1234;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Noise")
	double NoiseFrequency = 0.007;
};
