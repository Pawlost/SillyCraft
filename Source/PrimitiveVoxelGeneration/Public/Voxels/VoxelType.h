// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseWrapper.h"
#include "VoxelType.generated.h"

USTRUCT(BlueprintType)
struct PRIMITIVEVOXELGENERATION_API FVoxelType : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Basic")
	TObjectPtr<UMaterial> Material;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Basic")
	bool IsTransparent = false;
	
	// Max and min elevation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Surface")
	double SurfaceElevation = 1000;
	
	// Sea level is estimated at 0
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Surface")
	int64 RangeFromSeaLevel = 20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SurfaceNoise")
	int64 SurfaceSeed = 1234;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SurfaceNoise")
	EFastNoise_NoiseType SurfaceNoiseType = EFastNoise_NoiseType::ValueFractal;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SurfaceNoise")
	double SurfaceNoiseFrequency = 0.007;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReversedSurface")
	bool GenerateReversedSurface = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReversedSurface")
	double ReversedSurfaceDepth = 1500;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReversedSurface")
	int64 ReversedRangeFromSeaLevel = 20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReversedSurfaceNoise")
	int64 ReversedSurfaceSeed = 1234;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReversedSurfaceNoise")
	EFastNoise_NoiseType ReversedSurfaceNoiseType = EFastNoise_NoiseType::ValueFractal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ReversedSurfaceNoise")
	double ReversedSurfaceNoiseFrequency = 0.05;
};
