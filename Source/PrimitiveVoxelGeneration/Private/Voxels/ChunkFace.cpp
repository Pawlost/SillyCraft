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

bool FChunkFace::MergeFaceEndY(FChunkFace& prevFace, const FChunkFace& newFace)
{
	if(prevFace.Voxel == newFace.Voxel &&
		prevFace.EndVertexDown.Z == newFace.EndVertexDown.Z && prevFace.EndVertexDown.X == newFace.EndVertexDown.X &&
		prevFace.EndVertexUp.Z == newFace.EndVertexUp.Z && prevFace.EndVertexUp.X == newFace.EndVertexUp.X &&
		prevFace.EndVertexUp.Y == newFace.StartVertexUp.Y && prevFace.EndVertexUp.Y == newFace.StartVertexDown.Y )
	{
		prevFace.EndVertexDown = newFace.EndVertexDown;
		prevFace.EndVertexUp = newFace.EndVertexUp;
		return true;
	}
	return false;
}

bool FChunkFace::MergeFaceBeginY(FChunkFace& prevFace, const FChunkFace& newFace)
{
	if(prevFace.Voxel == newFace.Voxel &&
			prevFace.StartVertexDown.Z == newFace.StartVertexDown.Z && prevFace.StartVertexDown.X == newFace.StartVertexDown.X &&
			prevFace.StartVertexUp.Z == newFace.StartVertexUp.Z && prevFace.StartVertexUp.X == newFace.StartVertexUp.X &&
			prevFace.StartVertexUp.Y == newFace.EndVertexUp.Y && prevFace.StartVertexDown.Y == newFace.EndVertexDown.Y )
	{
		prevFace.StartVertexDown = newFace.StartVertexDown;
		prevFace.StartVertexUp = newFace.StartVertexUp;
		return true;
	}
	return false;
}

bool FChunkFace::MergeFaceBeginX(FChunkFace& prevFace, const FChunkFace& newFace)
{
	if(prevFace.Voxel == newFace.Voxel &&
			prevFace.StartVertexDown.Z == newFace.StartVertexDown.Z && prevFace.StartVertexDown.Y == newFace.StartVertexDown.Y &&
			prevFace.StartVertexUp.Z == newFace.StartVertexUp.Z && prevFace.StartVertexUp.Y == newFace.StartVertexUp.Y &&
			prevFace.StartVertexUp.X == newFace.EndVertexUp.X && prevFace.StartVertexDown.X == newFace.EndVertexDown.X )
	{
		prevFace.StartVertexDown = newFace.StartVertexDown;
		prevFace.StartVertexUp = newFace.StartVertexUp;
		return true;
	}
	return false;
}

bool FChunkFace::MergeFaceEndX(FChunkFace& prevFace, const FChunkFace& newFace)
{
	if(prevFace.Voxel == newFace.Voxel &&
			prevFace.EndVertexDown.Z == newFace.EndVertexDown.Z && prevFace.EndVertexDown.Y == newFace.EndVertexDown.Y &&
			prevFace.EndVertexUp.Z == newFace.EndVertexUp.Z && prevFace.EndVertexUp.Y == newFace.EndVertexUp.Y &&
			prevFace.EndVertexUp.X == newFace.StartVertexUp.X && prevFace.EndVertexUp.X == newFace.StartVertexDown.X )
	{
		prevFace.EndVertexDown = newFace.EndVertexDown;
		prevFace.EndVertexUp = newFace.EndVertexUp;
		return true;
	}
	return false;
}

bool FChunkFace::MergeFaceDownZ(FChunkFace& prevFace, const FChunkFace& newFace)
{
	if(prevFace.Voxel == newFace.Voxel &&
		prevFace.StartVertexDown.X == newFace.StartVertexDown.X && prevFace.StartVertexDown.Y == newFace.StartVertexDown.Y &&
		prevFace.EndVertexDown.X == newFace.EndVertexDown.X && prevFace.EndVertexDown.Y == newFace.EndVertexDown.Y &&
		prevFace.StartVertexDown.Z == newFace.StartVertexUp.Z && prevFace.EndVertexDown.Z == newFace.EndVertexUp.Z)
	{
		prevFace.StartVertexDown = newFace.StartVertexDown;
		prevFace.EndVertexDown = newFace.EndVertexDown;
		return true;
	}
	return false;
}

bool FChunkFace::MergeFaceDownX(FChunkFace& prevFace, const FChunkFace& newFace)
{
	if(prevFace.Voxel == newFace.Voxel &&
		prevFace.StartVertexDown.Z == newFace.StartVertexDown.Z && prevFace.StartVertexDown.Y == newFace.StartVertexDown.Y &&
		prevFace.EndVertexDown.Z == newFace.EndVertexDown.Z && prevFace.EndVertexDown.Y == newFace.EndVertexDown.Y &&
		prevFace.StartVertexDown.X == newFace.StartVertexUp.X && prevFace.EndVertexDown.X == newFace.EndVertexUp.X)
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