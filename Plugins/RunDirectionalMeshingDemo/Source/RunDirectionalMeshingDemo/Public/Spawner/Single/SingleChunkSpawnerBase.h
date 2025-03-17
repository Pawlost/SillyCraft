// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mesher/ChunkSpawnerBase.h"
#include "SingleChunkSpawnerBase.generated.h"

UCLASS(Abstract)
class RUNDIRECTIONALMESHINGDEMO_API ASingleChunkSpawnerBase : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	FIntVector SingleChunkGridPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	bool AlignGridPositionWithSpawner = true;

	virtual void ChangeVoxelInChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition,
									const FName& VoxelId) override;
	
protected:
	virtual void BeginPlay() override;
	
	TSharedPtr<FChunk> SingleChunk;
	
	virtual void StartMeshing() {};
};
