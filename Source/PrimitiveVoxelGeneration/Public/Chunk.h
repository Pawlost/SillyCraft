// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Meshers/ChunkMesherBase.h"
#include "Chunk.generated.h"

UCLASS()
class PRIMITIVEVOXELGENERATION_API AChunk : public AActor
{
GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AChunk();

	void SetMesherClass(const TSubclassOf<UChunkMesherBase>& ChunkMesherClass);
	TSubclassOf<UChunkMesherBase> GetMesherClass();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<UChunkMesherBase> ChunkMesherClass = nullptr;
	
	UPROPERTY()
	TObjectPtr<UChunkMesherBase> ChunkMesher = nullptr;

	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> MeshComponent = nullptr;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
