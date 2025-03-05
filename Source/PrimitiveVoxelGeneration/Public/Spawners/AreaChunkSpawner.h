// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "ChunkSpawnerBase.h"
#include "Meshers/MeshingStructs/GridDirectionToFace.h"
#include "Chunks/ChunkStruct.h"
#include "AreaChunkSpawner.generated.h"

//TODO: add forward declarations
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API AAreaChunkSpawner : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	int32 SpawnRadius = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	bool ShowBorderChunks = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	double DespawnRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	int32 ChunksAboveSpawner = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	int32 ChunksBelowSpawner = 0;
	
	virtual void ModifyVoxelAtChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition, const FVoxel& VoxelId) override;

protected:

	TMap<FIntVector, TSharedPtr<FChunkStruct>> ChunkGrid;
	
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GenerateChunks() override;

	TQueue<TSharedPtr<FChunkStruct>> DespawnedChunks;
	TQueue<TWeakObjectPtr<AChunkRmcActor>, EQueueMode::Mpsc> UnusedActors;
	
private:
	void GenerateChunkMesh(FChunkFaceParams& chunkParams, const TSharedPtr<FChunkStruct>& chunk);
	void AddChunkFromGrid(FChunkFaceParams& params, const FGridDirectionToFace& faceDirection);
	virtual void DespawnChunks() override;
	TFuture<void> SpawnHandle;
	TFuture<void> EditHandle;
};
