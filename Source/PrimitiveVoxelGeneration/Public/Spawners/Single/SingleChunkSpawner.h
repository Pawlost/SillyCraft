// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "SingleChunkSpawnerBase.h"
#include "SingleChunkSpawner.generated.h"
//TODO: add forward declarations

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API ASingleChunkSpawner : public ASingleChunkSpawnerBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void StartMeshing() override;
};
