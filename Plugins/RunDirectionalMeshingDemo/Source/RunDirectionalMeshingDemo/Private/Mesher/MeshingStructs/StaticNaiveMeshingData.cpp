#include "Mesher/MeshingStructs/StaticNaiveMeshingData.h"

FStaticNaiveMeshingData FStaticNaiveMeshingData::FrontFaceData = FStaticNaiveMeshingData(
	EFaceDirection::Front,
	FChunkFace::FrontFace,
	FChunkFace::MergeFaceEnd
);

FStaticNaiveMeshingData FStaticNaiveMeshingData::BackFaceData = FStaticNaiveMeshingData(
	EFaceDirection::Back,
	FChunkFace::BackFace,
	FChunkFace::MergeFaceStart
);

FStaticNaiveMeshingData FStaticNaiveMeshingData::RightFaceData = FStaticNaiveMeshingData(
	EFaceDirection::Right,
	FChunkFace::RightFace,
	FChunkFace::MergeFaceStart
);

FStaticNaiveMeshingData FStaticNaiveMeshingData::LeftFaceData = FStaticNaiveMeshingData(
	EFaceDirection::Left,
	FChunkFace::LeftFace,
	FChunkFace::MergeFaceEnd
);

FStaticNaiveMeshingData FStaticNaiveMeshingData::TopFaceData = FStaticNaiveMeshingData(
	EFaceDirection::Top,
	FChunkFace::TopFace,
	FChunkFace::MergeFaceEnd
);
FStaticNaiveMeshingData FStaticNaiveMeshingData::BottomFaceData = FStaticNaiveMeshingData(
	EFaceDirection::Bottom,
	FChunkFace::BottomFace,
	FChunkFace::MergeFaceStart
);