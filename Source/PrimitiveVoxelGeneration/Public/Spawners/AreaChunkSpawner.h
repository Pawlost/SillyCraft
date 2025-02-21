﻿// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "ChunkSpawnerBase.h"
#include "MeshingStructs/DirectionToFace.h"
#include "Voxels/ChunkStruct.h"
#include "AreaChunkSpawner.generated.h"

//TODO: add forward declarations
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API AAreaChunkSpawner : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation")
	int32 SpawnRadius = 2;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	TMap<FIntVector, TSharedPtr<FChunkStruct>> ChunkGrid;
	
private:
	void AddChunkFromGrid(FChunkFaceParams& params, const FDirectionToFace& faceDirection);
	void GenerateArea(const FIntVector& gridPosition);

	TFuture<void> Handle;
};
