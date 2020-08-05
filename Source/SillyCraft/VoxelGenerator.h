// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ChunkMesher.h"
#include "Chunk.h"
#include "CoreMinimal.h"
#include "BlockRegistry.h"
#include "Logging/LogVerbosity.h"
#include "GameFramework/Actor.h"
#include "VoxelGenerator.generated.h"

UCLASS()
class SILLYCRAFT_API AVoxelGenerator : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AVoxelGenerator();
	~AVoxelGenerator();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TMap<TTuple<int, int, int>, AChunk*> m_chunks;
	TArray<AChunk*> m_ungeneratedChunks;
	BlockRegistry* m_registry;
	ChunkMesher* m_mesher;

	void Refill();
};
