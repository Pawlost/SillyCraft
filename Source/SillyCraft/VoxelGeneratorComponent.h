// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ChunkMesher.h"
#include "Chunk.h"
#include <FastNoiseWrapper.h>
#include "BlockRegistry.h"
#include "CoreMinimal.h"
#include <mutex>
#include "Components/ActorComponent.h"
#include "VoxelGeneratorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SILLYCRAFT_API UVoxelGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVoxelGeneratorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TMap<TTuple<int, int, int>, AChunk*> m_chunks;
	BlockRegistry* m_registry;
	ChunkMesher* m_mesher;
	AChunk* m_currentChunk;
	AActor* m_owner;
	FVector m_lastPosition;

	bool GenerateChunk(AChunk& chunk);
	void ChangeZone(bool needspawn);
	void Refill();
};
