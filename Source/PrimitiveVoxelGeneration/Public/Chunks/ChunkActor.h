﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Chunks/ChunkBase.h"
#include "GameFramework/Actor.h"
#include "ChunkActor.generated.h"

UCLASS()
class PRIMITIVEVOXELGENERATION_API AChunkActor : public AActor
{
GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AChunkActor();

	void SetChunkSettings(const TSubclassOf<UChunkBase>& chunkClass, TSharedPtr<FUChunkSettings> chunkSettings);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<UChunkBase> ChunkClass = nullptr;
	
	UPROPERTY()
	TObjectPtr<UChunkBase> Chunk = nullptr;

	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> MeshComponent = nullptr;

	TSharedPtr<FUChunkSettings> ChunkSettings = nullptr;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
