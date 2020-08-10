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
	
	UPROPERTY(VisibleAnywhere, Category = Basic)
	int32 X;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	int32 Y;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	int32 Z;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TMap<int32, int32> ChangeIds;
};
