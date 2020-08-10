// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlockID.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct SILLYCRAFT_API FBlockID
{
	GENERATED_USTRUCT_BODY()
public:
	int BlockID;
	bool PlacedByPlayer;
};
