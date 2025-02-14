// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "FastNoiseWrapper.h"
#include "VoxelGeneratorBase.h"
#include "Components/ActorComponent.h"
#include "VoxelGridGenerator.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API UVoxelGridGenerator : public UVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	UVoxelGridGenerator();
	virtual void GenerateVoxels(FChunkStruct& chunk) override;

private:
	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> Noise;
	
	bool Empty = false;
};