#pragma once
#include "CoreMinimal.h"
#include "ChunkRMCActor.h"
#include "RunDirectionalMeshingDemo/Public/Voxel/Voxel.h"
#include "Chunk.generated.h"

USTRUCT()
struct RUNDIRECTIONALMESHINGDEMO_API FChunk
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AChunkRmcActor> ChunkMeshActor = nullptr;

	UPROPERTY()
	TArray<FVoxel> Voxels;

	//Key: voxel ID; Value: voxel count
	UPROPERTY()
	TMap<int32, uint32> ChunkVoxelTypeTable;

	UPROPERTY()
	FIntVector GridPosition;

	bool IsInitialized = false;
	bool HasMesh = false;
	bool IsActive = false;
};
