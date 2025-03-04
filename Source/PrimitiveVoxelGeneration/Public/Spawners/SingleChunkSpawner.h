﻿// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "ChunkSpawnerBase.h"
#include "Voxels/ChunkStruct.h"
#include "SingleChunkSpawner.generated.h"
//TODO: add forward declarations

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API ASingleChunkSpawner : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	TSharedPtr<FChunkStruct> SingleChunk;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	FIntVector SingleChunkGridPosition;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
