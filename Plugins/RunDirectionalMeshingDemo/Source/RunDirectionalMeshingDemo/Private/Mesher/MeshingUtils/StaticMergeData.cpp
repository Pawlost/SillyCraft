#include "Mesher/MeshingUtils/StaticMergeData.h"

FStaticMergeData FStaticMergeData::FrontFaceData = FStaticMergeData(
	EFaceDirection::Front,
	FChunkFace::FrontFace,
	FChunkFace::MergeFaceEnd
);

FStaticMergeData FStaticMergeData::BackFaceData = FStaticMergeData(
	EFaceDirection::Back,
	FChunkFace::BackFace,
	FChunkFace::MergeFaceStart
);

FStaticMergeData FStaticMergeData::RightFaceData = FStaticMergeData(
	EFaceDirection::Right,
	FChunkFace::RightFace,
	FChunkFace::MergeFaceStart
);

FStaticMergeData FStaticMergeData::LeftFaceData = FStaticMergeData(
	EFaceDirection::Left,
	FChunkFace::LeftFace,
	FChunkFace::MergeFaceEnd
);

FStaticMergeData FStaticMergeData::TopFaceData = FStaticMergeData(
	EFaceDirection::Top,
	FChunkFace::TopFace,
	FChunkFace::MergeFaceEnd
);

FStaticMergeData FStaticMergeData::BottomFaceData = FStaticMergeData(
	EFaceDirection::Bottom,
	FChunkFace::BottomFace,
	FChunkFace::MergeFaceStart
);
