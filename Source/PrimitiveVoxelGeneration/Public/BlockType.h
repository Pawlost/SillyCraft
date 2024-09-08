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
	FName Name;

	UPROPERTY()
	EHardness BlockHardness;

	UPROPERTY()
	int32 Id;

	UPROPERTY()
	int32 Range;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UMaterial> Material;

	UPROPERTY()
	int32 MinElevation;

	UPROPERTY()
	int32 MaxElevation;
};
