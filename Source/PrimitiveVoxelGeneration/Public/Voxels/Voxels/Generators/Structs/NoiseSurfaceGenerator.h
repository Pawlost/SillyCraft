// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseWrapper.h"
#include "NoiseSurfaceGenerator.generated.h"

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FNoiseSurfaceGenerator
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> SurfaceGenerator;

	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> ReverseSurfaceGenerator;

	FName VoxelName;
	FVoxelType VoxelType;
};