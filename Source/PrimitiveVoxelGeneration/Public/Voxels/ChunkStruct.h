#pragma once
#include "CoreMinimal.h"
#include "Chunks/ChunkRMCActor.h"
#include "ChunkStruct.generated.h"

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkStruct
{
    GENERATED_BODY()

	TWeakObjectPtr<AChunkRMCActor> ChildChunk;

	UPROPERTY()
	TArray<FVoxel> Voxels;

	UPROPERTY()
	TMap<int32, int16> ChunkVoxelTypeTable;
	
	FIntVector GridPosition;

	bool IsEmpty;
	bool IsGenerated;
};