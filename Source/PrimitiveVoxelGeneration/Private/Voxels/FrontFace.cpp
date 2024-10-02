#include "Voxels/FrontFace.h"

#include "Voxels/ChunkFace.h"

bool FFrontFace::NaiveFaceMerge(const UChunkFace& otherFace)
{
	return false;// FrontNaiveMerge(*this, otherFace);
}

bool FFrontFace::GreedyFaceMerge(const UChunkFace& otherFace)
{
	return UChunkFace::GreedyFaceMerge(otherFace);
}
