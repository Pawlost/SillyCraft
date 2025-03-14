#pragma once

#include "CoreMinimal.h"
#include "FastNoiseWrapper.h"
#include "VoxelType.generated.h"

USTRUCT(BlueprintType)
struct PRIMITIVEVOXELGENERATION_API FVoxelType : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Basic",
		meta = (ToolTip = "Material applied to the generated voxel."))
	TObjectPtr<UMaterial> Material = nullptr;

	UPROPERTY(EditAnywhere, Category = "Basic",
		meta = (ToolTip =
			"When false, voxel will not be generated but will remain interactable"))
	bool bShouldGenerate = true;

	UPROPERTY(EditAnywhere, Category = "Basic",
		meta = (ToolTip =
			"Specifies if the voxel surface is transparent. When true, additional mesh borders are created for opaque voxels."
		))
	bool bIsTransparent = false;

	UPROPERTY(EditAnywhere, Category = "Basic",
		meta = (EditCondition = "bShouldGenerate", ToolTip =
			"Determines whether generation of this voxel type stops at a certain depth. Useful for creating floating islands, caves or surface layers."
		))
	bool bGenerateReversedSurface = false;

	UPROPERTY(EditAnywhere, Category = "Surface",
		meta = (EditCondition = "bShouldGenerate", ToolTip =
			"Defines the minimum and maximum elevation of the surface."
		))
	double Surface_Elevation = 100.0;

	UPROPERTY(EditAnywhere, Category = "Surface",
		meta = (EditCondition = "bShouldGenerate", ToolTip =
			"Defines the additional distance from sea level at which the surface is positioned. Sea level is assumed to be at 0."
		))
	int64 Surface_DistanceFromSeaLevel = 20;

	UPROPERTY(EditAnywhere, Category = "SurfaceNoise", meta = (EditCondition = "bShouldGenerate"))
	int32 SurfaceNoise_Seed = 1234;

	UPROPERTY(EditAnywhere, Category = "SurfaceNoise", meta = (EditCondition = "bShouldGenerate"))
	EFastNoise_NoiseType SurfaceNoise_Type = EFastNoise_NoiseType::ValueFractal;

	UPROPERTY(EditAnywhere, Category = "SurfaceNoise", meta = (EditCondition = "bShouldGenerate"))
	float SurfaceNoise_Frequency = 0.007f;

	UPROPERTY(EditAnywhere, Category = "ReversedSurface",
		meta = (EditCondition = "bGenerateReversedSurface && bShouldGenerate", ToolTip =
			"Defines the minimum and maximum depth of the reversed surface."
		))
	double ReversedSurface_Depth = 150.0;

	UPROPERTY(EditAnywhere, Category = "ReversedSurface",
		meta = (EditCondition = "bGenerateReversedSurface && bShouldGenerate", ToolTip =
			"Defines the additional distance from sea level at which the surface is positioned. Sea level is assumed to be at 0. Generation of reversed surface must be enabled."
		))
	int64 ReversedSurface_DistanceFromSeaLevel = 20;

	UPROPERTY(EditAnywhere, Category = "ReversedSurfaceNoise",
		meta = (EditCondition = "bGenerateReversedSurface && bShouldGenerate"))
	int32 ReversedSurfaceNoise_Seed = 1234;

	UPROPERTY(EditAnywhere, Category = "ReversedSurfaceNoise",
		meta = (EditCondition = "bGenerateReversedSurface && bShouldGenerate"))
	EFastNoise_NoiseType ReversedSurfaceNoise_Type = EFastNoise_NoiseType::ValueFractal;

	UPROPERTY(EditAnywhere, Category = "ReversedSurfaceNoise",
		meta = (EditCondition = "bGenerateReversedSurface && bShouldGenerate"))
	float ReversedSurfaceNoise_Frequency = 0.05f;
};
