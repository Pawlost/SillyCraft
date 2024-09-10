// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "Components/ActorComponent.h"
#include "VoxelGeneratorComponent.generated.h"

using namespace UE::Math;

//TODO: add forward declarations
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API UVoxelGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UVoxelGeneratorComponent();
	
   	//if set will overide all chunk meshers
   	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation");
    TSubclassOf<UChunkMesherBase> DefaultChunkMesher = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation");
	int64 Seed = 1234;

	// How many layers of chunks are generated around player
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation|Blocks");
	int32 GenerationDistance = 1;

	//Height is same in as Width
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation|Blocks");
	int32 ChunkWidthInBlocks = 4;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation|Blocks");
    int32 BlockSize = 100;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug");
	float DebugTime = 0.005f;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual bool IsPlayerInChunkBounds() const;
	
	virtual void DespawnChunks(const FIntVector ChunkMinDistance, const FIntVector ChunkMaxDistance);
    virtual void SpawnChunks(const FIntVector ChunkMinDistance, const FIntVector ChunkMaxDistance);
	
private:
	void ShowDebugVector(TVector<double>& vector, FColor color);
	void UpdateCurrentChunkLocation();

	const double ChunkSize = BlockSize * ChunkWidthInBlocks;
	const double RenderDistanceBounds = ChunkSize * GenerationDistance;

	struct CurrentChunkLocation
	{
		TVector<double> Coords = TVector(0.0);
		FIntVector ChunkMinCoords = FIntVector(0);
		FIntVector ChunkMaxCoords = FIntVector(0);
	};

	CurrentChunkLocation CurrentChunkLocation;
	
	TMap<FIntVector, AChunk*> SpawnedChunks = TMap<FIntVector, AChunk*>();
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
