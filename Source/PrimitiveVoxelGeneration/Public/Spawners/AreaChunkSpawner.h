// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Chunks/ChunkMesherBase.h"
#include "Voxels/ChunkStruct.h"
#include "AreaChunkSpawner.generated.h"

//TODO: add forward declarations
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API AAreaChunkSpawner : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation")
	TSubclassOf<UChunkMesherBase> ChunkMesherTemplate = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	TMap<FIntVector, FChunkStruct> ChunkGrid;
};
