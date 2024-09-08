// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlockType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EHardness : uint8
{
	Empty, Soft, Medium, Hard
};

USTRUCT(BlueprintType)
struct PRIMITIVEVOXELGENERATION_API FBlockType : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString BlockName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EHardness BlockHardness;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Range;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UMaterial> Material;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MinSpawnElevation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxSpawnElevation;
};
