﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RealtimeMeshActor.h"
#include "Components/ActorComponent.h"
#include "ChunkRMCActor.generated.h"

UCLASS()
class AChunkRmcActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChunkRmcActor();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RealtimeMesh")
	TObjectPtr<URealtimeMeshComponent> RealtimeMeshComponent;
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
};
