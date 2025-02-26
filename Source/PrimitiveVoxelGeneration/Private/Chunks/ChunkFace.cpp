#include "Chunks/ChunkFace.h"

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

FVector3f FChunkFace::GetFinalStartVertexDown(const double& voxelSize) const
{
	return static_cast<FVector3f>(StartVertexDown) * voxelSize;
}

FVector3f FChunkFace::GetFinalStartVertexUp(const double& voxelSize) const
{
	return static_cast<FVector3f>(StartVertexUp) * voxelSize;
}

FVector3f FChunkFace::GetFinalEndVertexDown(const double& voxelSize) const
{
	return static_cast<FVector3f>(EndVertexDown) * voxelSize;
}

FVector3f FChunkFace::GetFinalEndVertexUp(const double& voxelSize) const
{
	return static_cast<FVector3f>(EndVertexUp) * voxelSize;
}

FChunkFace FChunkFace::CreateFrontFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, FrontFace);
}

FChunkFace FChunkFace::CreateBackFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, BackFace);
}

FChunkFace FChunkFace::CreateLeftFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, LeftFace);
}

FChunkFace FChunkFace::CreateRightFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, RightFace);
}

FChunkFace FChunkFace::CreateTopFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, TopFace);
}

FChunkFace FChunkFace::CreateBottomFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, BottomFace);
}

bool FChunkFace::MergeFaceEnd(FChunkFace& prevFace, const FChunkFace& newFace)
{
	if (prevFace.EndVertexDown == newFace.StartVertexDown &&
		prevFace.EndVertexUp == newFace.StartVertexUp)
	{
		prevFace.EndVertexDown = newFace.EndVertexDown;
		prevFace.EndVertexUp = newFace.EndVertexUp;
		return true;
	}
	return false;
}

bool FChunkFace::MergeFaceStart(FChunkFace& prevFace, const FChunkFace& newFace)
{
	if (prevFace.StartVertexUp == newFace.EndVertexUp &&
		prevFace.StartVertexDown == newFace.EndVertexDown)
	{
		prevFace.StartVertexDown = newFace.StartVertexDown;
		prevFace.StartVertexUp = newFace.StartVertexUp;
		return true;
	}
	return false;
}

bool FChunkFace::MergeFaceUp(FChunkFace& prevFace, const FChunkFace& newFace)
{
	if (prevFace.StartVertexUp == newFace.StartVertexDown &&
		prevFace.EndVertexUp == newFace.EndVertexDown)
	{
		prevFace.StartVertexUp = newFace.StartVertexUp;
		prevFace.EndVertexUp = newFace.EndVertexUp;
		return true;
	}
	return false;
}

FChunkFace FChunkFace::CreateChunkFace(const FIntVector& InitialPosition, const FVoxel& voxel, FChunkFace face)
{
	face.Voxel = voxel;
	face += InitialPosition;
	return face;
}

void operator+=(FChunkFace& ChunkFace, const FIntVector& Vector)
{
	ChunkFace.StartVertexDown += Vector;
	ChunkFace.StartVertexUp += Vector;
	ChunkFace.EndVertexDown += Vector;
	ChunkFace.EndVertexUp += Vector;
}

void operator-=(FChunkFace& ChunkFace, const FIntVector& Vector)
{
	ChunkFace.StartVertexDown -= Vector;
	ChunkFace.StartVertexUp -= Vector;
	ChunkFace.EndVertexDown -= Vector;
	ChunkFace.EndVertexUp -= Vector;
}

FChunkFace operator-(FChunkFace ChunkFace, const FIntVector& Vector)
{
	ChunkFace -= Vector;
	return ChunkFace;
}
