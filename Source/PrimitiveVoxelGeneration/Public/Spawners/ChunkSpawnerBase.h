// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Meshers/MesherBase.h"
#include "Chunks/ChunkStruct.h"
#include "ChunkSpawnerBase.generated.h"

//TODO: add forward declarations
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API AChunkSpawnerBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	TSubclassOf<UMesherBase> MesherBlueprint = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	double DespawnRadius;

	UFUNCTION(BlueprintCallable)
	void MoveSpawnToPosition(const FVector& newPosition);

	UFUNCTION(BlueprintCallable)
	double GetHighestElevationAtLocation(const FVector& location) const;

	UFUNCTION(BlueprintCallable)
	virtual void ChangeVoxelAt(const FVector& hitPosition, const FVector& hitNormal, const int32 VoxelId) {}
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	TFuture<void> SpawnChunk(const TSharedPtr<FChunkStruct>& chunk);
	static void AddSideChunk(FChunkFaceParams& chunkParams, EFaceDirection direction,
	                         const TSharedPtr<FChunkStruct>& chunk);

	virtual void GenerateChunks()
	{
	}

	virtual void DespawnChunks()
	{
	}

	FIntVector CenterGridPosition;

	UPROPERTY()
	UMesherBase* ChunkMesher;

	FIntVector WorldPositionToChunkGridPosition(const FVector& worldPosition) const;
};
