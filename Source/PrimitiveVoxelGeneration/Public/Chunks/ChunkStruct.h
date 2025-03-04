#pragma once
#include "CoreMinimal.h"
#include "VoxelIdInChunk.h"
#include "Chunks/ChunkRMCActor.h"
#include "Voxels/Voxel.h"
#include "ChunkStruct.generated.h"

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkStruct
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AChunkRmcActor> ChunkMeshActor;

	UPROPERTY()
	TArray<FVoxel> Voxels;
	
	TMap<int32, FVoxelIdInChunk> ChunkVoxelTypeTable;

	UPROPERTY()
	FIntVector GridPosition;

	bool IsInitialized = false;
};
