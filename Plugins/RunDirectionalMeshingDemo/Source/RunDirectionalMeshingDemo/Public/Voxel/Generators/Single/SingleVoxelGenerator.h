// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "SingleVoxelGeneratorBase.h"
#include "Components/ActorComponent.h"
#include "SingleVoxelGenerator.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class RUNDIRECTIONALMESHINGDEMO_API USingleVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	virtual void GenerateVoxels(FChunkStruct& chunk) override;
};
