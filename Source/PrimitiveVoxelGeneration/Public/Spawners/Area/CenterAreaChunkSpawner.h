// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "RealTimeChunkSpawnerBase.h"
#include "CenterAreaChunkSpawner.generated.h"

//TODO: add forward declarations
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API ACenterAreaChunkSpawner : public ARealTimeChunkSpawnerBase
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GenerateArea() override;
	static void WaitForAllTasks(TArray<TSharedFuture<void>>& tasks);
};
