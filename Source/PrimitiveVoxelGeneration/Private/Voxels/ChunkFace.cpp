#include "Voxels/ChunkFace.h"

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

FVector FChunkFace::GetFinalStartVertexDown(const double& voxelSize) const
{
	return static_cast<FVector>(StartVertexDown) * voxelSize;
}

FVector FChunkFace::GetFinalStartVertexUp(const double& voxelSize) const
{
	return static_cast<FVector>(StartVertexUp) * voxelSize;
}

FVector FChunkFace::GetFinalEndVertexDown(const double& voxelSize) const
{
	return static_cast<FVector>(EndVertexDown) * voxelSize;
}

FVector FChunkFace::GetFinalEndVertexUp(const double& voxelSize) const
{
	return static_cast<FVector>(EndVertexUp) * voxelSize;
}

FChunkFace FChunkFace::CreateFrontFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, FrontFace);
}

FChunkFace FChunkFace::CreateBackFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel,BackFace);
}

FChunkFace FChunkFace::CreateLeftFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel,LeftFace);
}

FChunkFace FChunkFace::CreateRightFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel,RightFace);
}

FChunkFace FChunkFace::CreateTopFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel,TopFace);
}

FChunkFace FChunkFace::CreateBottomFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel,BottomFace);
}

bool FChunkFace::MergeFaceEnd(FChunkFace& prevFace, const FChunkFace& newFace)
{
	if(prevFace.EndVertexDown == newFace.StartVertexDown &&
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
	if(prevFace.StartVertexUp == newFace.EndVertexUp &&
		prevFace.StartVertexDown == newFace.EndVertexDown )
	{
		prevFace.StartVertexDown = newFace.StartVertexDown;
		prevFace.StartVertexUp = newFace.StartVertexUp;
		return true;
	}
	return false;
}

bool FChunkFace::MergeFaceDown(FChunkFace& prevFace, const FChunkFace& newFace)
{
	if(prevFace.StartVertexDown == newFace.StartVertexUp &&
		prevFace.EndVertexDown == newFace.EndVertexUp)
	{
		prevFace.StartVertexDown = newFace.StartVertexDown;
		prevFace.EndVertexDown = newFace.EndVertexDown;
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