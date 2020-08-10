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

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FVector PlayerPosition;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FRotator PlayerRotation;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	TArray<FPrimitiveChunk> SavedChunks;
};
