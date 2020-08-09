// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrimitiveChunk.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct SILLYCRAFT_API FPrimitiveChunk
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	int X;

	UPROPERTY()
	int Y;

	UPROPERTY()
	int Z;

	UPROPERTY()
	TMap<int, int> ChangeIds;
};
