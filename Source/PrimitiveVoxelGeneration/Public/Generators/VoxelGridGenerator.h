﻿// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "FastNoiseVoxelGenerator.h"
#include "Components/ActorComponent.h"
#include "VoxelGridGenerator.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API UVoxelGridGenerator : public UFastNoiseVoxelGenerator
{
	GENERATED_BODY()

public:
	virtual void GenerateVoxels(FChunkStruct& chunk) override;
private:
	bool Empty = false;
};