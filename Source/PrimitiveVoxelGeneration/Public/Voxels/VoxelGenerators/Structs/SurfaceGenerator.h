// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoiseSurfaceGenerator.h"
#include "SurfaceGenerator.generated.h"

USTRUCT()
struct FSurfaceGenerator
{
	GENERATED_BODY()

	UPROPERTY()
	FNoiseSurfaceGenerator SurfaceGenerator;

	UPROPERTY()
	FNoiseSurfaceGenerator ReverseSurfaceGenerator;
	
	bool GenerateReverseSurface = false;
	bool IsTransparent = false;
};