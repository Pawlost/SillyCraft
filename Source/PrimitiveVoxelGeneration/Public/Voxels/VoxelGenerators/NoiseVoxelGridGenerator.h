﻿// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "VoxelGeneratorBase.h"
#include "Components/ActorComponent.h"
#include "Chunks/ChunkStruct.h"
#include "Structs/NoiseGeneratorVariables.h"
#include "NoiseVoxelGridGenerator.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class PRIMITIVEVOXELGENERATION_API UNoiseVoxelGridGenerator : public UVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	virtual void GenerateVoxels(FChunkStruct& chunk) override;
	virtual double GetHighestElevationAtLocation(const FVector& location) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TArray<FNoiseGeneratorVariables> NoiseGenerators;
};
