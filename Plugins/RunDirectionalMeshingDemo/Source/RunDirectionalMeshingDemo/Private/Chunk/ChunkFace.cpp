#include "Chunk/ChunkFace.h"

// Initialized quad vertices

FChunkFace FChunkFace::FrontFace = FChunkFace(
	FIntVector(0, 0, 0),
	FIntVector(0, 1, 0),
	FIntVector(0, 1, 1),
	FIntVector(0, 0, 1));

FChunkFace FChunkFace::BackFace = FChunkFace(
	FIntVector(1, 1, 0),
	FIntVector(1, 0, 0),
	FIntVector(1, 0, 1),
	FIntVector(1, 1, 1));

FChunkFace FChunkFace::RightFace = FChunkFace(
	FIntVector(1, 0, 0),
	FIntVector(0, 0, 0),
	FIntVector(0, 0, 1),
	FIntVector(1, 0, 1));

FChunkFace FChunkFace::LeftFace = FChunkFace(
	FIntVector(0, 1, 0),
	FIntVector(1, 1, 0),
	FIntVector(1, 1, 1),
	FIntVector(0, 1, 1));

FChunkFace FChunkFace::TopFace = FChunkFace(
	FIntVector(0, 0, 1),
	FIntVector(0, 1, 1),
	FIntVector(1, 1, 1),
	FIntVector(1, 0, 1));

FChunkFace FChunkFace::BottomFace = FChunkFace(
	FIntVector(0, 1, 0),
	FIntVector(0, 0, 0),
	FIntVector(1, 0, 0),
	FIntVector(1, 1, 0));

FChunkFace FChunkFace::CreateChunkFace(const FIntVector& InitialPosition, const FVoxel& Voxel, FChunkFace Face)
{
	// Combine voxel and static quad definition
	Face.Voxel = Voxel;
	Face.StartVertexDown += InitialPosition;
	Face.StartVertexUp += InitialPosition;
	Face.EndVertexDown += InitialPosition;
	Face.EndVertexUp += InitialPosition;
	return Face;
}

FChunkFace FChunkFace::CreateFrontFace(const FIntVector& InitialPosition, const FVoxel& Voxel)
{
	return CreateChunkFace(InitialPosition, Voxel, FrontFace);
}

FChunkFace FChunkFace::CreateBackFace(const FIntVector& InitialPosition, const FVoxel& Voxel)
{
	return CreateChunkFace(InitialPosition, Voxel, BackFace);
}

FChunkFace FChunkFace::CreateLeftFace(const FIntVector& InitialPosition, const FVoxel& Voxel)
{
	return CreateChunkFace(InitialPosition, Voxel, LeftFace);
}

FChunkFace FChunkFace::CreateRightFace(const FIntVector& InitialPosition, const FVoxel& Voxel)
{
	return CreateChunkFace(InitialPosition, Voxel, RightFace);
}

FChunkFace FChunkFace::CreateTopFace(const FIntVector& InitialPosition, const FVoxel& Voxel)
{
	return CreateChunkFace(InitialPosition, Voxel, TopFace);
}

FChunkFace FChunkFace::CreateBottomFace(const FIntVector& InitialPosition, const FVoxel& Voxel)
{
	return CreateChunkFace(InitialPosition, Voxel, BottomFace);
}

/**
 * Compare vertices and merge quads
 * @return true if previous face was merged
 */
bool FChunkFace::MergeFaceEnd(FChunkFace& PrevFace, const FChunkFace& NewFace)
{
	if (PrevFace.EndVertexDown == NewFace.StartVertexDown &&
		PrevFace.EndVertexUp == NewFace.StartVertexUp)
	{
		PrevFace.EndVertexDown = NewFace.EndVertexDown;
		PrevFace.EndVertexUp = NewFace.EndVertexUp;
		return true;
	}
	return false;
}

/**
 * Compare vertices and merge quads
 * @return true if previous face was merged
 */
bool FChunkFace::MergeFaceStart(FChunkFace& PrevFace, const FChunkFace& NewFace)
{
	if (PrevFace.StartVertexUp == NewFace.EndVertexUp &&
		PrevFace.StartVertexDown == NewFace.EndVertexDown)
	{
		PrevFace.StartVertexDown = NewFace.StartVertexDown;
		PrevFace.StartVertexUp = NewFace.StartVertexUp;
		return true;
	}
	return false;
}

/**
 * Compare vertices and merge quads
 * @return true if previous face was merged
 */
bool FChunkFace::MergeFaceUp(FChunkFace& PrevFace, const FChunkFace& NewFace)
{
	if (PrevFace.StartVertexUp == NewFace.StartVertexDown &&
		PrevFace.EndVertexUp == NewFace.EndVertexDown)
	{
		PrevFace.StartVertexUp = NewFace.StartVertexUp;
		PrevFace.EndVertexUp = NewFace.EndVertexUp;
		return true;
	}
	return false;
}

FVector3f FChunkFace::GetFinalStartVertexDown(const double& VoxelSize) const
{
	return static_cast<FVector3f>(StartVertexDown) * VoxelSize;
}

FVector3f FChunkFace::GetFinalStartVertexUp(const double& VoxelSize) const
{
	return static_cast<FVector3f>(StartVertexUp) * VoxelSize;
}

FVector3f FChunkFace::GetFinalEndVertexDown(const double& VoxelSize) const
{
	return static_cast<FVector3f>(EndVertexDown) * VoxelSize;
}

FVector3f FChunkFace::GetFinalEndVertexUp(const double& voxelSize) const
{
	return static_cast<FVector3f>(EndVertexUp) * voxelSize;
}
