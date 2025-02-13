// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Chunks/ChunkMesherBase.h"
#include "Voxels/ChunkStruct.h"
#include "ChunkSpawnerBase.generated.h"

//TODO: add forward declarations
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API AChunkSpawnerBase : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation")
	TSubclassOf<UChunkMesherBase> ChunkMesherTemplate = nullptr;

protected:
	void SpawnChunk(FChunkStruct& Chunk, const FVector& location);
};