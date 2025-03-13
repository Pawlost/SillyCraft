#pragma once
#include "FaceDirection.h"
#include "Chunks/ChunkFace.h"
#include "Chunks/ChunkStruct.h"
#include "Spawners/ChunkSpawnerBase.h"
#include "ChunkFaceParams.generated.h"

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkParams
{
	GENERATED_BODY()

	TSharedPtr<FChunkStruct> SideChunks[FACE_SIDE_COUNT];
	TSharedPtr<FChunkStruct> OriginalChunk;

	UPROPERTY()
	TWeakObjectPtr<AChunkSpawnerBase> SpawnerPtr = nullptr;

	FAttachmentTransformRules ActorAttachmentRules = FAttachmentTransformRules::KeepWorldTransform; 
	bool ShowBorders = false;
	bool ExecutedOnMainThread = false;
};

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkFaceParams
{
	GENERATED_BODY()

	TArray<TSharedPtr<TArray<FChunkFace>>> Faces[FACE_SIDE_COUNT];
	FChunkParams ChunkParams;
	TMap<uint16, uint16> VoxelIdToLocalVoxelMap; 
};
