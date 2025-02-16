// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Chunks/ChunkMesherBase.h"
#include "Generators/VoxelGridGenerator.h"
#include "Voxels/ChunkStruct.h"
#include "ChunkSpawnerBase.generated.h"

//TODO: add forward declarations
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API AChunkSpawnerBase : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	TSubclassOf<UChunkMesherBase> ChunkMesherBlueprint = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	FIntVector GridPosition;

	// Allows selecting a component class in Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TSubclassOf<UVoxelGeneratorBase> VoxelGeneratorClass = nullptr;

protected:
	void SpawnChunk(FChunkStruct& Chunk, const FIntVector& location);

	UPROPERTY()
	UVoxelGeneratorBase* VoxelGridGenerator;

	UPROPERTY()
	UChunkMesherBase* ChunkMesherBase;
};