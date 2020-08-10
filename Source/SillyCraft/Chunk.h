// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.h"
#include "Constants.h"
#include "BlockRegistry.h"
#include "BlockID.h"
#include <array>
#include "GameFramework/Actor.h"
#include <ProceduralMeshComponent.h>
#include "Chunk.generated.h"

UCLASS()
class SILLYCRAFT_API AChunk : public AActor
{
	GENERATED_BODY()
		
private:
	std::array<FBlockID, Constants::ChunkSize3D>* m_blockIDs;
	UFastNoiseWrapper* m_noise;
	BlockRegistry* m_registry;
	UProceduralMeshComponent* m_mesh;
	UMaterial* m_material;
	bool m_hasMesh = false;

public:	

	// Sets default values for this actor's properties
	AChunk();
	~AChunk();
	bool Generated = false;

	int GetBlockID(const int& index) const;
	bool IsPlaceByPlayer(const int& index) const;
	void ChangeBlockID(const int& index, const int& id, bool changedByPlayer);
	void BaseFill();
	void Fill(const int& blockID);
	void Initialize(BlockRegistry* registry, UMaterial* material);

	void Show();
	void Hide();

	bool HasMesh();

	void SetMesh(const int& index, const TArray<FVector>& vectors, const TArray<int32>& indice, const TArray<FLinearColor>& color);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	

 	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
