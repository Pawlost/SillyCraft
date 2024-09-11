// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FastNoiseWrapper.h"
#include "NoiseGeneratorBase.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVEVOXELGENERATION_API UNoiseGeneratorBase : public UObject
{
	GENERATED_BODY()

	public:

		UPROPERTY()
		UFastNoiseWrapper* noise;

		void GenerateNoise();
	
};
