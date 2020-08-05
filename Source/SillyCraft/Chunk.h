// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.h"
#include "GameFramework/Actor.h"
#include <ProceduralMeshComponent.h>
#include "Chunk.generated.h"

UCLASS()
class SILLYCRAFT_API AChunk : public AActor
{
	GENERATED_BODY()
private:
	int* m_blockIDs;
public:	
	// Sets default values for this actor's properties
	AChunk();
	~AChunk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite)
	UProceduralMeshComponent* Mesh;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Fill(const Block& block);
	void Fill(Block* block);
};
