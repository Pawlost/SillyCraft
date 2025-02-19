// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Meshers/MesherBase.h"
#include "Voxels/ChunkStruct.h"
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
	FIntVector SingleChunkGridPosition;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SpawnChunk(FChunkStruct& Chunk, const FIntVector& location);
	static void AddSideChunk(FChunkFaceParams& Chunk, EFaceDirection direction, FChunkStruct* chunk);
	
	UPROPERTY()
	UMesherBase* ChunkMesher;
};