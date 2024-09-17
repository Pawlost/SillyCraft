// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "ChunkActor.generated.h"

struct FChunkSettings;
class UChunkBase;

UCLASS()
class PRIMITIVEVOXELGENERATION_API AChunkActor : public AActor
{
public:
	TWeakObjectPtr<UProceduralMeshComponent> GetProceduralMeshComponent() const;

private:
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChunkActor();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent = nullptr;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
