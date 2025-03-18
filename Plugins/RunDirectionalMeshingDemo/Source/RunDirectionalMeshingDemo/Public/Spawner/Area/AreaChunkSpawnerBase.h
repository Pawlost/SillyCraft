#pragma once
#include "CoreMinimal.h"
#include "Spawner/ChunkSpawnerBase.h"
#include "AreaChunkSpawnerBase.generated.h"

struct FFaceToDirection;
class AChunkRMCActor;

UCLASS(Abstract)
class RUNDIRECTIONALMESHINGDEMO_API AAreaChunkSpawnerBase : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chunk")
	int32 SpawnZone = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool SpawnCenterChunk = true;

	virtual void ChangeVoxelInChunk(const FIntVector& ChunkGridPosition, const FIntVector& VoxelPosition,
	                                const FName& VoxelId) override;

protected:
	TMap<FIntVector, TSharedPtr<FChunk>> ChunkGrid;
	bool ShowChunkBorders = false;
	virtual void BeginPlay() override;

	TQueue<TWeakObjectPtr<AChunkRMCActor>, EQueueMode::Mpsc> UnusedActors;

	virtual void GenerateArea() PURE_VIRTUAL(AAreaChunkSpawnerBase::GenerateArea)

	void GenerateChunkMesh(FMesherVariables& MesherVars, const FIntVector& ChunkGridPosition);
	virtual void SpawnChunk(const FIntVector& ChunkGridPosition, TSharedFuture<void>* OutAsyncExecution = nullptr);

	virtual void SpawnChunks() override;

private:
	void AddChunkFromGrid(FMesherVariables& MesherVars, const FFaceToDirection& FaceDirection);

	TSharedFuture<void> EditHandle;
	FCriticalSection Mutex;
};
