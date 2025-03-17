#pragma once
#include "CoreMinimal.h"
#include "Spawner/ChunkSpawnerBase.h"
#include "AreaChunkSpawnerBase.generated.h"

class AChunkRMCActor;

UCLASS(Abstract)
class RUNDIRECTIONALMESHINGDEMO_API AAreaChunkSpawnerBase : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	int32 SpawnZone = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool SpawnCenterChunk = true;

	virtual void ChangeVoxelInChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition,
	                                const FName& VoxelId) override;

protected:
	TMap<FIntVector, TSharedPtr<FChunk>> ChunkGrid;
	bool ShowChunkBorders = false;

	// Called when the game starts
	virtual void BeginPlay() override;

	TQueue<TSharedPtr<FChunk>> DespawnedChunks;
	TQueue<TWeakObjectPtr<AChunkRMCActor>, EQueueMode::Mpsc> UnusedActors;

	virtual void GenerateArea()
	{
	}

	void GenerateChunkMesh(FMesherVariables& chunkParams, const FIntVector& chunkGridPosition);
	void SpawnChunk(const FIntVector& chunkGridPosition, TSharedFuture<void>* asyncExecution = nullptr);
	virtual void SpawnChunks() override;

private:
	void AddChunkFromGrid(FMesherVariables& params, const FFaceToDirection& faceDirection);

	TSharedFuture<void> EditHandle;
	FCriticalSection Mutex;
};
