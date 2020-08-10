// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ChunkMesher.h"
#include "Chunk.h"
#include <FastNoiseWrapper.h>
#include "BlockRegistry.h"
#include "PrimitiveChunk.h"
#include "Save.h"
#include "GameFramework/SaveGame.h"
#include "CoreMinimal.h"
#include "InteractionParticles.h"
#include "FastCube.h"
#include "Kismet/GameplayStatics.h"
#include <mutex>
#include "Components/ActorComponent.h"
#include "VoxelGeneratorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SILLYCRAFT_API UVoxelGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVoxelGeneratorComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Save");
	int UserIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Save");
	FString SlotName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Materials");
	UMaterial* Material;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Particles");
	UParticleSystem* Particles;

	UFUNCTION(BlueprintCallable, Category = "VoxelInteraction")
	void HighlightTargetBlock(const bool& hit, FVector location, const FVector& normal);

	UFUNCTION(BlueprintCallable, Category = "VoxelInteraction")
	void Pick(const bool& hit, FVector location, const FVector& normal);

	UFUNCTION(BlueprintCallable, Category = "VoxelInteraction")
	void Place(const bool& hit, const FVector& location, const FVector& normal);

	UFUNCTION(BlueprintCallable, Category = "VoxelInteraction")
	FRotator GetSavedRotation();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TMap<TTuple<int, int, int>, AChunk*> m_chunks;
	BlockRegistry* m_registry;
	ChunkMesher* m_mesher;
	AChunk* m_currentChunk;
	AActor* m_owner;
	AInteractionParticles* m_particles;
	Block* m_damagedblock;
	Block* m_holdingblock;
	FVector m_lastPosition;
	std::mutex m_mutex;
	FTimerHandle m_timer;
	AFastCube* m_highlightCube;
	USave* m_save;
	void DestroyParticles();
	void ChunkChanged(int index, int value, AChunk* chunk);

	void ChangeZone(bool needspawn);
};
