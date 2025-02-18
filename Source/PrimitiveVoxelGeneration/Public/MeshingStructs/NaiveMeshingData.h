#pragma once
#include "StaticNaiveMeshingData.h"

struct FNaiveMeshingData
{
	const FStaticNaiveMeshingData& StaticMeshingData;
	int32 ForwardVoxelIndex;
	int32 ChunkBorderIndex;
	int32 PreviousVoxelIndex;

	explicit FNaiveMeshingData(FStaticNaiveMeshingData& staticMeshingData) : StaticMeshingData(staticMeshingData),
	                                                                         ForwardVoxelIndex(0), ChunkBorderIndex(0), PreviousVoxelIndex(0)
	{
	}
};
