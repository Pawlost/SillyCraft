// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spawners/ChunkSpawnerBase.h"
#include "SingleChunkSpawnerBase.generated.h"

UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API ASingleChunkSpawnerBase : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	FIntVector SingleChunkGridPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	bool AlignGridPositionWithSpawner = true;

protected:
	virtual void BeginPlay() override;
	
	TSharedPtr<FChunkStruct> SingleChunk;
	
	virtual void ModifyVoxelAtChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition,
	                                const FVoxel& VoxelId) override;
	
	virtual void StartMeshing() {};
};
