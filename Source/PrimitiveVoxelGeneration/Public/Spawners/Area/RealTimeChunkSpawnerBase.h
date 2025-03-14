#pragma once
#include "CoreMinimal.h"
#include "AreaChunkSpawnerBase.h"
#include "RealTimeChunkSpawnerBase.generated.h"

//TODO: add forward declarations
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API ARealTimeChunkSpawnerBase : public AAreaChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	int32 DespawnZone = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	int32 ChunksAboveSpawner = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	int32 ChunksBelowSpawner = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	int32 BufferZone = 1;
	
	UFUNCTION(BlueprintCallable)
	void MoveGridCenterToPosition(const FVector& newPosition);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void DespawnChunks() override;
	FCriticalSection m_CriticalSection;
};
