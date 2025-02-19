#pragma once
#include "FaceSide.h"
#include "Voxels/ChunkFace.h"

struct FStaticNaiveMeshingData
{
	const EFaceDirection faceSide;
	const FChunkFace& faceTemplate;
	const TFunctionRef<bool(FChunkFace& prevFace, const FChunkFace& newFace)> naiveFaceMerge;
	
	FStaticNaiveMeshingData(const EFaceDirection faceSide, const FChunkFace& faceTemplate, const TFunctionRef<bool(FChunkFace& prevFace, const FChunkFace& newFace)>& naiveFaceMerge)
	:
		faceSide(faceSide),
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
