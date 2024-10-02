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

bool FChunkFace::IsAxisStable(const FIntVector& mergeVertex, const FIntVector& otherMergeVertex,
		const EUnstableAxis unstableAxis)
{
	switch (unstableAxis)
	{
	case EUnstableAxis::X:
		return mergeVertex.Z == otherMergeVertex.Z && mergeVertex.Y == otherMergeVertex.Y;
	case EUnstableAxis::Y:
		return mergeVertex.Z == otherMergeVertex.Z && mergeVertex.X == otherMergeVertex.X;
	case EUnstableAxis::Z:
		return mergeVertex.Y == otherMergeVertex.Y && mergeVertex.X == otherMergeVertex.X;
	default:
		return false;
	}
}

bool FChunkFace::MergeFace(const FChunkFace& otherFace, const EMergeMethod mergeMethod, const EUnstableAxis unstableAxis)
{
	switch (mergeMethod)
	{
	case EMergeMethod::Begin:
		if(IsAxisStable(StartVertexDown, otherFace.StartVertexDown, unstableAxis) &&
			IsAxisStable(StartVertexUp, otherFace.StartVertexUp, unstableAxis))
		{
			StartVertexDown = otherFace.StartVertexDown;
			StartVertexUp = otherFace.StartVertexUp;
			return true;
		}
		return false;
		
	case EMergeMethod::End:

		if(IsAxisStable(EndVertexDown, otherFace.EndVertexDown, unstableAxis) &&
			IsAxisStable(EndVertexUp, otherFace.EndVertexUp, unstableAxis))
		{
			EndVertexDown = otherFace.EndVertexDown;
			EndVertexUp = otherFace.EndVertexUp;
			return true;
		}
		return false;

	case EMergeMethod::Up:
		if(IsAxisStable(StartVertexUp, otherFace.StartVertexUp, unstableAxis) &&
			IsAxisStable(EndVertexUp, otherFace.EndVertexUp, unstableAxis))
		{
			EndVertexUp = otherFace.EndVertexUp;
			StartVertexUp = otherFace.StartVertexUp;
			return true;
		}
		return false;
		
	case EMergeMethod::Down:
		if(IsAxisStable(StartVertexDown, otherFace.StartVertexDown, unstableAxis) &&
			IsAxisStable(EndVertexDown, otherFace.EndVertexDown, unstableAxis))
		{
			StartVertexDown = otherFace.StartVertexDown;
			EndVertexDown = otherFace.EndVertexDown;
			return true;
		}
		return false;
		
	default:
		return false;
	}
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