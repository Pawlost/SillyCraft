// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "FastNoiseWrapper.h"
#include "VoxelGeneratorBase.h"

//TODO: specify class
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API UFastNoiseVoxelGenerator : public UVoxelGeneratorBase
{
	GENERATED_BODY()

	public:
		UFastNoiseVoxelGenerator();
	
	protected:
		UPROPERTY()
		TObjectPtr<UFastNoiseWrapper> Noise;
};