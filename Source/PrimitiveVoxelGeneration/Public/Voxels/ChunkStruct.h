﻿#pragma once
#include "CoreMinimal.h"
#include "Voxel.h"
#include "Chunks/ChunkRMCActor.h"
#include "ChunkStruct.generated.h"

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkStruct
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AChunkRmcActor> ChunkMeshActor;

	UPROPERTY()
	TArray<FVoxel> Voxels;

	UPROPERTY()
	TMap<int32, int16> ChunkVoxelTypeTable;

	UPROPERTY()
	FIntVector GridPosition;

	bool IsEmpty = true;
	bool HasMesh = false;
};
