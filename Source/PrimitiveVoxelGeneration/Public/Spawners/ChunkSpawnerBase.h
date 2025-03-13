﻿// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Meshers/MesherBase.h"
#include "Chunks/ChunkStruct.h"
#include "Meshers/MeshingStructs/FaceDirection.h"
#include "ChunkSpawnerBase.generated.h"

//TODO: add forward declarations
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API AChunkSpawnerBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	TSubclassOf<UVoxelGeneratorBase> VoxelGeneratorBlueprint = nullptr;

	UFUNCTION(BlueprintCallable)
	void MoveSpawnToPosition(const FVector& newPosition);

	UFUNCTION(BlueprintCallable)
	double GetHighestElevationAtLocation(const FVector& location) const;

	UFUNCTION(BlueprintCallable)
	void ChangeVoxelAt(const FVector& hitPosition, const FVector& hitNormal,
	const FVoxel& VoxelId, bool place);
	
	UFUNCTION(BlueprintCallable)
	virtual void ModifyVoxelAtChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition, const FVoxel& VoxelId) {};
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	static void AddSideChunk(FChunkFaceParams& chunkParams, EFaceDirection direction,
	                         const TSharedPtr<FChunkStruct>& chunk);

	
	void InitChunk(TSharedPtr<FChunkStruct>& chunk,
								  const FIntVector& gridPosition, TSharedFuture<void>* asyncExecution = nullptr) const;
	
	virtual void SpawnChunks()
	{
	}

	virtual void DespawnChunks()
	{
	}

	FIntVector CenterGridPosition;

	UPROPERTY()
	TObjectPtr<UVoxelGeneratorBase> VoxelGenerator;

	FIntVector WorldPositionToChunkGridPosition(const FVector& worldPosition) const;
};
