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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation")
	int32 SpawnRadius = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation")
	bool ShowBorderChunks = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	double DespawnRadius;
	
	virtual void ModifyVoxelAtChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition, const FVoxel& VoxelId) override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	TMap<FIntVector, TSharedPtr<FChunkStruct>> ChunkGrid;
	void GenerateChunkMesh(FChunkFaceParams& chunkParams, const TSharedPtr<FChunkStruct>& chunk);
	
private:
	void AddChunkFromGrid(FChunkFaceParams& params, const FGridDirectionToFace& faceDirection);
	virtual void GenerateChunks() override;
	virtual void DespawnChunks() override;
	TFuture<void> SpawnHandle;
	TFuture<void> EditHandle;
};
