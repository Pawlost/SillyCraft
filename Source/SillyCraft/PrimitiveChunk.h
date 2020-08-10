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
	
	UPROPERTY(SaveGame)
	int X;

	UPROPERTY(SaveGame)
	int Y;

	UPROPERTY(SaveGame)
	int Z;

	UPROPERTY(SaveGame)
	TMap<int, int> ChangeIds;
};
