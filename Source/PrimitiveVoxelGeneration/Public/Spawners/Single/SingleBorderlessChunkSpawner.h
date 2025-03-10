// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Meshers/MeshingStructs/GridDirectionToFace.h"
#include "Chunks/ChunkStruct.h"
#include "Spawners/ChunkSpawnerBase.h"
#include "SingleBorderlessChunkSpawner.generated.h"
//TODO: add forward declarations

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API ASingleBorderlessChunkSpawner : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	TSharedPtr<FChunkStruct> SingleChunk;
	TSharedPtr<FChunkStruct> SideChunk[FACE_SIDE_COUNT];

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	FIntVector SingleChunkGridPosition;
	
	virtual void ModifyVoxelAtChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition, const FVoxel& VoxelId) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	FChunkFaceParams ChunkParams;
	void SpawnSideChunk(const FGridDirectionToFace& faceDirection);
};
