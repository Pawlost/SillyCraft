﻿#pragma once
#include "FaceDirection.h"
#include "Chunk/ChunkFace.h"
#include "Chunk/Chunk.h"
#include "MesherVariables.generated.h"

class AChunkSpawnerBase;

USTRUCT()
struct RUNDIRECTIONALMESHINGDEMO_API FChunkParams
{
	GENERATED_BODY()

	TSharedPtr<FChunk> SideChunks[CHUNK_FACE_COUNT];
	TSharedPtr<FChunk> OriginalChunk;

	UPROPERTY()
	TWeakObjectPtr<AChunkSpawnerBase> SpawnerPtr = nullptr;

	bool LocalTransform = false;
	bool ShowBorders = false;
	bool ExecutedOnMainThread = false;
};

USTRUCT()
struct RUNDIRECTIONALMESHINGDEMO_API FMesherVariables
{
	GENERATED_BODY()

	TArray<TSharedPtr<TArray<FChunkFace>>> Faces[CHUNK_FACE_COUNT];
	FChunkParams ChunkParams;
	TMap<uint16, uint16> VoxelIdToLocalVoxelMap; 
};
