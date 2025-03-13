// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Meshers/MeshingStructs/GridDirectionToFace.h"
#include "Chunks/ChunkStruct.h"
#include "Spawners/ChunkSpawnerBase.h"
#include "AreaChunkSpawnerBase.generated.h"

//TODO: add forward declarations
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API AAreaChunkSpawnerBase : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	int32 SpawnZone = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	int32 BufferZone = 1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	int32 DespawnZone = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	int32 ChunksAboveSpawner = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	int32 ChunksBelowSpawner = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool SpawnCenterChunk = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool WorldCenter = true;
	
	virtual void ModifyVoxelAtChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition, const FVoxel& VoxelId) override;

protected:

	TMap<FIntVector, TSharedPtr<FChunkStruct>> ChunkGrid;
	
	// Called when the game starts
	virtual void BeginPlay() override;

	TQueue<TSharedPtr<FChunkStruct>> DespawnedChunks;
	TQueue<TWeakObjectPtr<AChunkRmcActor>, EQueueMode::Mpsc> UnusedActors;
	virtual void GenerateArea(){}
	
	void GenerateChunkMesh(FChunkFaceParams& chunkParams, const FIntVector& chunkGridPosition);
	void SpawnChunk(const FIntVector& chunkGridPosition, TSharedFuture<void>* asyncExecution = nullptr);
	
private:
	void AddChunkFromGrid(FChunkFaceParams& params, const FGridDirectionToFace& faceDirection);
	
	virtual void DespawnChunks() override;
	virtual void SpawnChunks() override;
	
	TSharedFuture<void> SpawnHandle;
	TSharedFuture<void> EditHandle;
	FCriticalSection Mutex;
};
