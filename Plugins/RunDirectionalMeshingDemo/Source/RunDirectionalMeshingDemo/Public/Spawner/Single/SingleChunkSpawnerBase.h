#pragma once

#include "CoreMinimal.h"
#include "Spawner/ChunkSpawnerBase.h"
#include "SingleChunkSpawnerBase.generated.h"

UCLASS(ClassGroup=(ChunkSpawners), Abstract)
class RUNDIRECTIONALMESHINGDEMO_API ASingleChunkSpawnerBase : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	FIntVector SingleChunkGridPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	bool AlignGridPositionWithSpawner = true;

	virtual void ChangeVoxelInChunk(const FIntVector& ChunkGridPosition, const FIntVector& VoxelPosition,
									const FName& VoxelId) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void StartMeshing() PURE_VIRTUAL(ASingleChunkSpawnerBase::StartMeshing)
	
	TSharedPtr<FChunk> SingleChunk;

private:
	FCriticalSection CritSection;
};
