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

	UPROPERTY()
	FVector PlayerPosition;

	UPROPERTY()
	FRotator PlayerRotation;

	UPROPERTY()
	TArray<FPrimitiveChunk> SavedChunks;

	void Init();
	void Deinit();

	TMap<TTuple<int, int, int>, FPrimitiveChunk> ChunkMap;
};
