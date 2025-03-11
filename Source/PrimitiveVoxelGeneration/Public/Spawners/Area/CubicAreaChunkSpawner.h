// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "AreaChunkSpawnerBase.h"
#include "CubicAreaChunkSpawner.generated.h"

//TODO: add forward declarations
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API ACubicAreaChunkSpawner : public AAreaChunkSpawnerBase
{
	GENERATED_BODY()

protected:
	virtual void GenerateArea() override;
};
