#pragma once
#include "FaceDirection.h"
#include "Chunk/ChunkFace.h"

struct FStaticMergeData
{
	const EFaceDirection FaceSide;
	const FChunkFace& FaceTemplate;
	const TFunctionRef<bool(FChunkFace& PrevFace, const FChunkFace& NewFace)> RunDirectionFaceMerge;

	static FStaticMergeData FrontFaceData;
	static FStaticMergeData BackFaceData;
	static FStaticMergeData LeftFaceData;
	static FStaticMergeData RightFaceData;
	static FStaticMergeData TopFaceData;
	static FStaticMergeData BottomFaceData;
};
