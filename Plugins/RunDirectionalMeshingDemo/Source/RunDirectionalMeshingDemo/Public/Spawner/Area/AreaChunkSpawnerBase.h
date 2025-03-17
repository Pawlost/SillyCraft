#pragma once
#include "CoreMinimal.h"
#include "Mesher/ChunkSpawnerBase.h"
#include "Mesher/MeshingStructs/GridDirectionToFace.h"
#include "AreaChunkSpawnerBase.generated.h"

class AChunkRmcActor;
//TODO: add forward declarations
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
	TQueue<TWeakObjectPtr<AChunkRmcActor>, EQueueMode::Mpsc> UnusedActors;

	virtual void GenerateArea()
	{
	}

	void GenerateChunkMesh(FMesherVariables& chunkParams, const FIntVector& chunkGridPosition);
	void SpawnChunk(const FIntVector& chunkGridPosition, TSharedFuture<void>* asyncExecution = nullptr);
	virtual void SpawnChunks() override;

private:
	void AddChunkFromGrid(FMesherVariables& params, const FGridDirectionToFace& faceDirection);

	TSharedFuture<void> EditHandle;
	FCriticalSection Mutex;
};
