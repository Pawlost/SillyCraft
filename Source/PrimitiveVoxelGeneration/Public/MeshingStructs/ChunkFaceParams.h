#pragma once
#include "FaceSide.h"
#include "Voxels/ChunkFace.h"
#include "Voxels/ChunkStruct.h"
#include "ChunkFaceParams.generated.h"

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkParams
{
	GENERATED_BODY()

	FChunkStruct* SideChunks[FACE_SIDE_COUNT];
	FChunkStruct* OriginalChunk;
};

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkFaceParams
{
	GENERATED_BODY()

	TArray<TSharedPtr<TArray<FChunkFace>>> Faces[FACE_SIDE_COUNT];
	FChunkParams ChunkParams;
};
