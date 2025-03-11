// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "AreaChunkSpawnerBase.h"
#include "CenterAreaChunkSpawner.generated.h"

//TODO: add forward declarations
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API ACenterAreaChunkSpawner : public AAreaChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	int32 SpawnRadius = 2;

protected:
	virtual void GenerateArea() override;

private:
	void SpawnSideChunk(const FIntVector& centerPosition, TArray<FIntVector>& SpawnPositionsSet,
	                    FGridDirectionToFace direction, const double& maxSpawnDistance);
};
