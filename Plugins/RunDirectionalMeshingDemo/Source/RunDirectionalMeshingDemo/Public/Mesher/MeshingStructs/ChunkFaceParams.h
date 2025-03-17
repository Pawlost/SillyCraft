#pragma once
#include "FaceDirection.h"
#include "Chunk/ChunkFace.h"
#include "Chunk/ChunkStruct.h"
#include "ChunkFaceParams.generated.h"

class AChunkSpawnerBase;

USTRUCT()
struct RUNDIRECTIONALMESHINGDEMO_API FChunkParams
{
	GENERATED_BODY()

	TSharedPtr<FChunkStruct> SideChunks[FACE_SIDE_COUNT];
	TSharedPtr<FChunkStruct> OriginalChunk;

	UPROPERTY()
	TWeakObjectPtr<AChunkSpawnerBase> SpawnerPtr = nullptr;

	bool WorldTransform = false;
	bool ShowBorders = false;
	bool ExecutedOnMainThread = false;
};

USTRUCT()
struct RUNDIRECTIONALMESHINGDEMO_API FChunkFaceParams
{
	GENERATED_BODY()

	TArray<TSharedPtr<TArray<FChunkFace>>> Faces[FACE_SIDE_COUNT];
	FChunkParams ChunkParams;
	TMap<uint16, uint16> VoxelIdToLocalVoxelMap; 
};
