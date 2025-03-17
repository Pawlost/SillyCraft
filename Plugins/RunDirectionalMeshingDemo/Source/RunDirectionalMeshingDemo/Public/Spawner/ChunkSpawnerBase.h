#pragma once
#include "CoreMinimal.h"
#include "Mesher/MeshingUtils/FaceDirection.h"
#include "Voxel/Generators/VoxelGeneratorBase.h"
#include "ChunkSpawnerBase.generated.h"

UCLASS(ClassGroup=(ChunkSpawners), Abstract, Blueprintable)
class RUNDIRECTIONALMESHINGDEMO_API AChunkSpawnerBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowAbstract="false", BlueprintBaseOnly), NoClear,
		Category="Chunk")
	TSubclassOf<UVoxelGeneratorBase> VoxelGeneratorBlueprint = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool LocalChunkTransform = false;

	UFUNCTION(BlueprintCallable)
	double GetHighestElevationAtLocation(const FVector& Location) const;

	UFUNCTION(BlueprintCallable)
	void ChangeVoxelAtHit(const FVector& HitPosition, const FVector& HitNormal,
	                      const FName& VoxelName, bool bPick);

	UFUNCTION(BlueprintCallable)
	virtual void ChangeVoxelInChunk(const FIntVector& ChunkGridPosition, const FIntVector& VoxelPosition,
	                                const FName& VoxelName) PURE_VIRTUAL(AChunkSpawnerBase::ChangeVoxelInChunk)

protected:
	virtual void BeginPlay() override;

	static void AddSideChunk(FMesherVariables& ChunkParams, EFaceDirection Direction,
	                         const TSharedPtr<FChunk>& Chunk);


	void AddChunkToGrid(TSharedPtr<FChunk>& Chunk,
	                    const FIntVector& GridPosition, TSharedFuture<void>* AsyncExecution = nullptr) const;

	virtual void SpawnChunks() PURE_VIRTUAL(AChunkSpawnerBase::SpawnChunks)
	virtual void DespawnChunks() PURE_VIRTUAL(AChunkSpawnerBase::DespawnChunks)

	UPROPERTY()
	TObjectPtr<UVoxelGeneratorBase> VoxelGenerator;

	FIntVector WorldPositionToChunkGridPosition(const FVector& WorldPosition) const;

	FIntVector CenterGridPosition;
};
