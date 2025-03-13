// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseWrapper.h"
#include "NoiseSurfaceGenerator.generated.h"

USTRUCT(BlueprintType)
struct FNoiseSurfaceGenerator
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> SurfaceNoise;
	
	uint32 MaxElevation;
	uint32 RangeFromSeaLevel;
};