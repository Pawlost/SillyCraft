﻿// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "RealtimeMeshActor.h"
#include "Chunks/ChunkMesherBase.h"
#include "Components/ActorComponent.h"
#include "AreaGenerator.generated.h"
struct FVoxelType;
using namespace UE::Math;
//TODO: add forward declarations

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API AAreaGenerator : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation")
	TSubclassOf<UChunkMesherBase> ChunkMesherTemplate = nullptr;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void GenerateMesh();
	void SpawnChunk(FVector location);
};