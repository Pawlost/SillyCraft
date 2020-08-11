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
	std::unique_ptr<std::array<FBlockID, Constants::ChunkSize3D>> m_blockIDs;
	std::shared_ptr<BlockRegistry> m_registry;

	UMaterial* m_material;
	bool m_hasMesh = false;

	UPROPERTY()
	UFastNoiseWrapper* m_noise;

	UPROPERTY()
	UProceduralMeshComponent* m_mesh;

public:	

	//Noise
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Configuration");
	int Seed = 1334;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Configuration");
	EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::PerlinFractal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Configuration");
	float NoiseFrequency = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Configuration");
	int MaxElevation = 20;

	AChunk();
	~AChunk();
	bool Generated = false;

	int GetBlockID(const int& index) const;
	bool IsPlaceByPlayer(const int& index) const;
	void ChangeBlockID(const int& index, const int& id, bool changedByPlayer);
	void BaseFill();
	void Fill(const int& blockID);
	void Initialize(std::shared_ptr<BlockRegistry> registry, UMaterial* material, const int& seed, const EFastNoise_NoiseType& noiseType, const float& noiseFrequency, const int& maxElevation);

	void Show();
	void Hide();

	bool HasMesh() const;

	void SetMesh(const int& index, const TArray<FVector>& vectors, const TArray<int32>& indice, const TArray<FLinearColor>& color);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	

 	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
