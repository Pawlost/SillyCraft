#pragma once
#include "Voxels/ChunkFace.h"

struct FStaticNaiveMeshingData
{
	FIntVector borderChunkDirection;
	const FChunkFace& faceTemplate;
	TFunctionRef<bool(FChunkFace& prevFace, const FChunkFace& newFace)> naiveFaceMerge;
	
	FStaticNaiveMeshingData(FIntVector borderChunkDirection, const FChunkFace& faceTemplate, TFunctionRef<bool(FChunkFace& prevFace, const FChunkFace& newFace)> naiveFaceMerge)
	:
		borderChunkDirection(borderChunkDirection),
		faceTemplate(faceTemplate),
		naiveFaceMerge(naiveFaceMerge)
	{
	}

	static FStaticNaiveMeshingData FrontFaceData;
	static FStaticNaiveMeshingData BackFaceData;
	static FStaticNaiveMeshingData LeftFaceData;
	static FStaticNaiveMeshingData RightFaceData;
	static FStaticNaiveMeshingData TopFaceData;
	static FStaticNaiveMeshingData BottomFaceData;
};
