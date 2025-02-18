#include "MeshingStructs/StaticNaiveMeshingData.h"

FStaticNaiveMeshingData FStaticNaiveMeshingData::FrontFaceData = FStaticNaiveMeshingData(
	FIntVector(-1, 0, 0),
	FChunkFace::FrontFace,
	FChunkFace::MergeFaceEnd
);

FStaticNaiveMeshingData FStaticNaiveMeshingData::BackFaceData = FStaticNaiveMeshingData(
	FIntVector(1, 0, 0),
	FChunkFace::BackFace,
	FChunkFace::MergeFaceStart
);

FStaticNaiveMeshingData FStaticNaiveMeshingData::RightFaceData = FStaticNaiveMeshingData(
	FIntVector(0, -1, 0),
	FChunkFace::RightFace,
	FChunkFace::MergeFaceStart
);

FStaticNaiveMeshingData FStaticNaiveMeshingData::LeftFaceData = FStaticNaiveMeshingData(
	FIntVector(0, 1, 0),
	FChunkFace::LeftFace,
	FChunkFace::MergeFaceEnd
);

FStaticNaiveMeshingData FStaticNaiveMeshingData::TopFaceData = FStaticNaiveMeshingData(
	FIntVector(0, 0, 1),
	FChunkFace::TopFace,
	FChunkFace::MergeFaceEnd
);
FStaticNaiveMeshingData FStaticNaiveMeshingData::BottomFaceData = FStaticNaiveMeshingData(
	FIntVector(0, 0, -1),
	FChunkFace::BottomFace,
	FChunkFace::MergeFaceStart
);