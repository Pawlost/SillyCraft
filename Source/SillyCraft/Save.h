// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	//UPROPERTY();
	//TArray<AChunk> SavedChunks;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FVector PlayerPosition;
};
