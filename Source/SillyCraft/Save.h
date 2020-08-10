// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrimitiveChunk.h"
#include "Chunk.h"
#include "GameFramework/SaveGame.h"
#include "Save.generated.h"

/**
 * 
 */
UCLASS()
class SILLYCRAFT_API USave : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY(SaveGame)
	FVector PlayerPosition;

	UPROPERTY(SaveGame)
	FRotator PlayerRotation;

	UPROPERTY(SaveGame)
	TArray<FPrimitiveChunk> SavedChunks;
};
