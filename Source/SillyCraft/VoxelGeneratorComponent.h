// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ChunkMesher.h"
#include "Chunk.h"
#include "BlockRegistry.h"
#include "PrimitiveChunk.h"
#include "Save.h"
#include "GameFramework/SaveGame.h"
#include "CoreMinimal.h"
#include "InteractionParticles.h"
#include "FastCube.h"
#include "Kismet/GameplayStatics.h"
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
	void HighlightTargetBlock(const bool& hit, const FVector& hitLocation, const FVector& hitNormal);

	UFUNCTION(BlueprintCallable, Category = "VoxelInteraction")
	void Pick(const bool& hit, const FVector& hitLocation, const FVector& hitNormal);

	UFUNCTION(BlueprintCallable, Category = "VoxelInteraction")
	void Place(const bool& hit, const FVector& hitLocation, const FVector& hitNormal);

	UFUNCTION(BlueprintCallable, Category = "Save")
	FVector GetSavedLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	FRotator GetSavedRotation() const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SavePosition(const FVector& location, const FRotator& rotation);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	const std::shared_ptr<BlockRegistry> m_registry;
	const std::shared_ptr<ChunkMesher> m_mesher;
	
	FTimerHandle m_timer;
	AActor* m_owner;
	TMap<TTuple<int, int, int>, AChunk*> m_chunks;
	AInteractionParticles* m_particles;
	std::shared_ptr<Block> m_damagedblock;
	std::shared_ptr<Block> m_holdingblock;
	FVector m_lastPosition;
	AFastCube* m_highlightCube;
	TMap<TTuple<int, int, int>, FPrimitiveChunk> m_savedChanges;

	UPROPERTY(SaveGame)
	USave* m_save;

	void DestroyParticles();

	void ChunkChanged(const int& index, const int& value, AChunk& chunk);

	void ChangeZone(bool needspawn, const FVector& position);

	void FillZone(const int& x, const int& y, const int& z, const TTuple<int, int, int>& pos);

	void GetChunkNeighbors(const int& x, const int& y, const int& z, std::array<AChunk*, 6>& outSideChunks) const;

	void CalculateHitCoords(int& x, int& y, int& z, int& chunkX, int& chunkY, int& chunkZ, const FVector& hitLocation) const;

	bool InZone(const int& posX, const int& posY, const int& posZ,const int& baseX, const int& baseY, const int& baseZ, const int& size) const;

	bool OutZone(const int& posX, const int& posY, const int& posZ, const int& baseX, const int& baseY, const int& baseZ, const int& size) const;
};
