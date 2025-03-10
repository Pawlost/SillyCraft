#pragma once
#include "CoreMinimal.h"
#include "Chunks/ChunkRMCActor.h"
#include "Voxels/Voxel.h"
#include "ChunkStruct.generated.h"

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkStruct
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AChunkRmcActor> ChunkMeshActor = nullptr;

	UPROPERTY()
	TArray<FVoxel> Voxels;

	//Key: voxel ID; Value: voxel count
	TMap<int32, uint32> ChunkVoxelTypeTable;

	UPROPERTY()
	FIntVector GridPosition;

	bool IsInitialized = false;
	bool HasMesh = false;
	bool IsActive = false;
};
