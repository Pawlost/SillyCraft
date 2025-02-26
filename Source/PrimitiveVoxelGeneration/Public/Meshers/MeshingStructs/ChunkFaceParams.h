#pragma once
#include "FaceDirection.h"
#include "Chunks/ChunkFace.h"
#include "Chunks/ChunkStruct.h"
#include "ChunkFaceParams.generated.h"

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkParams
{
	GENERATED_BODY()

	TSharedPtr<FChunkStruct> SideChunks[FACE_SIDE_COUNT];
	TSharedPtr<FChunkStruct> OriginalChunk;
};

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkFaceParams
{
	GENERATED_BODY()

	TArray<TSharedPtr<TArray<FChunkFace>>> Faces[FACE_SIDE_COUNT];
	FChunkParams ChunkParams;
};
