// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkMesherBase.h"
#include "RealtimeMeshActor.h"
#include "Components/ActorComponent.h"
#include "ChunkRMCActor.generated.h"

UCLASS()
class AChunkRMCActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RealtimeMesh")
	TObjectPtr<URealtimeMeshComponent> RealtimeMeshComponent;
	// Sets default values for this actor's properties
	AChunkRMCActor();
	void InitParameters(TSubclassOf<UChunkMesherBase> chunkMesherBase) const;
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
private:
	bool IsGenerated;
	bool IsEmpty;
};