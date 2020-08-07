// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.h"
#include "GameFramework/Actor.h"
#include <array>
#include <string>
#include <ProceduralMeshComponent.h>
#include "Chunk.generated.h"

UCLASS()
class SILLYCRAFT_API AChunk : public AActor
{
	GENERATED_BODY()
		
private:
	std::array<int, Constants::ChunkSize3D>* m_blockIDs;
	UFastNoiseWrapper* m_noise;

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
	int GetBlockID(const int& index) const;
	void BaseFill(int baseID, int airID);
	void Fill(const Block* block, const int& range);
	bool Generated = false;
};
