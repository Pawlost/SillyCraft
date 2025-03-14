#pragma once
#include "CoreMinimal.h"
#include "Meshers/MeshingStructs/GridDirectionToFace.h"
#include "Chunks/ChunkStruct.h"
#include "Spawners/ChunkSpawnerBase.h"
#include "AreaChunkSpawnerBase.generated.h"

//TODO: add forward declarations
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API AAreaChunkSpawnerBase : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	int32 SpawnZone = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool SpawnCenterChunk = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool WorldCenter = true;

	virtual void ChangeVoxelInChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition,
	                                const FName& VoxelId) override;

protected:
	TMap<FIntVector, TSharedPtr<FChunkStruct>> ChunkGrid;
	bool ShowChunkBorders = false;

	// Called when the game starts
	virtual void BeginPlay() override;

	TQueue<TSharedPtr<FChunkStruct>> DespawnedChunks;
	TQueue<TWeakObjectPtr<AChunkRmcActor>, EQueueMode::Mpsc> UnusedActors;

	virtual void GenerateArea()
	{
	}

	void GenerateChunkMesh(FChunkFaceParams& chunkParams, const FIntVector& chunkGridPosition);
	void SpawnChunk(const FIntVector& chunkGridPosition, TSharedFuture<void>* asyncExecution = nullptr);
	virtual void SpawnChunks() override;

private:
	void AddChunkFromGrid(FChunkFaceParams& params, const FGridDirectionToFace& faceDirection);

	TSharedFuture<void> EditHandle;
	FCriticalSection Mutex;
};
