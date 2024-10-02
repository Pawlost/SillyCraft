#pragma once
#include "ChunkFace.h"

class FFrontFace : public UChunkFace
{
public:
	virtual bool NaiveFaceMerge(const UChunkFace& otherFace) override;
	virtual bool GreedyFaceMerge(const UChunkFace& otherFace) override;

	FFrontFace( const FIntVector& InitialPosition, const FVoxel& voxel) : UChunkFace(
	FIntVector(0, 0, 0),
	FIntVector(0, 1, 0),
	FIntVector(0, 1, 1),
	FIntVector(0, 0, 1),
	voxel, InitialPosition)
	{
	}
};
