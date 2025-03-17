// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Mesher/MeshingStructs/FaceDirection.h"
#include "Voxel/Generators/VoxelGeneratorBase.h"
#include "ChunkSpawnerBase.generated.h"

//TODO: add forward declarations
UCLASS(Abstract)
class RUNDIRECTIONALMESHINGDEMO_API AChunkSpawnerBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	TSubclassOf<UVoxelGeneratorBase> VoxelGeneratorBlueprint = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool WorldCenter = true;
	
	UFUNCTION(BlueprintCallable)
	double GetHighestElevationAtLocation(const FVector& location) const;

	UFUNCTION(BlueprintCallable)
	void ChangeVoxelAtHit(const FVector& hitPosition, const FVector& hitNormal,
	const FName& VoxelName, bool pick);
	
	UFUNCTION(BlueprintCallable)
	virtual void ChangeVoxelInChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition, const FName& VoxelName) {};
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	static void AddSideChunk(FMesherVariables& chunkParams, EFaceDirection direction,
	                         const TSharedPtr<FChunk>& chunk);

	
	void InitChunk(TSharedPtr<FChunk>& chunk,
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
