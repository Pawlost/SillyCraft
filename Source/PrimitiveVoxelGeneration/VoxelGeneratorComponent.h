// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunks/ChunkActor.h"
#include "Chunks/ChunkGridData.h"
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
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation");
	int64 Seed = 1234;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation|Noise");
	double MaximumElevation = 2000.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation|Noise");
	double NoiseFrequency = 0.001;
	
   	//if set will overide all chunk meshers
   	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation");
    TSubclassOf<UChunkBase> ChunkTemplate = nullptr;

	// How many layers of chunks are generated around player
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation|Blocks");
	int32 GenerationDistance = 2;

	//Height is same in as Width
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation|Blocks");
	int32 ChunkSideSizeInVoxels = 32;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation|Blocks");
    int32 VoxelSize = 20;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug");
	float DebugTime = 0.005f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation");
	FDataTableRowHandle VoxelTypeTable;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual bool IsPlayerInChunkBounds() const;
	
	virtual void DespawnChunks(const FIntVector ChunkMinDistance, const FIntVector ChunkMaxDistance);
    virtual void SpawnChunks(const FIntVector ChunkMinDistance, const FIntVector ChunkMaxDistance);
	
private:
	void RemoveChunk(const TTuple<TIntVector3<int>, AChunkActor*>& Element) const;
	void ShowDebugVector(TVector<double>& vector, FColor color);
	void UpdateCurrentChunkLocation();

	double ChunkSize = 0;
	double RenderDistanceBounds = 0;

	struct CurrentChunkLocation
	{
		TVector<double> Coords = TVector(0.0);
		FIntVector ChunkMinCoords = FIntVector(0);
		FIntVector ChunkMaxCoords = FIntVector(0);
	};

	CurrentChunkLocation CurrentChunkLocation;
	
	// The reason why shared pointer is used instead of game instance is so multiple moving  voxel generating actors can exist in a same scene with different settings.
	TSharedPtr<FChunkGridData> ChunkGridPtr;
	TSharedPtr<TMap<FIntVector, AChunkActor*>> SpawnedChunks;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
