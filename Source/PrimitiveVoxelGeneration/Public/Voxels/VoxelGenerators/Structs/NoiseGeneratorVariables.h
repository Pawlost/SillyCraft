// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseWrapper.h"
#include "NoiseGeneratorVariables.generated.h"

USTRUCT()
struct FNoiseGeneratorVariables
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> SurfaceGenerator;

	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> ReverseSurfaceGenerator;

	uint32 MaxElevation;
	uint32 MaxDepth;
	
	bool GenerateReverseSurface;
	bool IsTransparent = false;
};