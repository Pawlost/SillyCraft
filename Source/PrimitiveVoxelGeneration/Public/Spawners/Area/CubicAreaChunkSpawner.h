// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "RealTimeChunkSpawnerBase.h"
#include "CubicAreaChunkSpawner.generated.h"

//TODO: add forward declarations
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API ACubicAreaChunkSpawner : public ARealTimeChunkSpawnerBase
{
	GENERATED_BODY()

protected:
	virtual void GenerateArea() override;
};
