#include "Voxels/ChunkFace.h"

/*
ChunkFace ChunkFace::BackFace = ChunkFace(
	FIntVector(1, 1, 0),
	FIntVector(1, 0, 0),
	FIntVector(1, 0, 1),
	FIntVector(1, 1, 1));

ChunkFace ChunkFace::RightFace = ChunkFace(
	FIntVector(1, 0, 0),
	FIntVector(0, 0, 0),
	FIntVector(0, 0, 1),
	FIntVector(1, 0, 1));

ChunkFace ChunkFace::LeftFace = ChunkFace(
	FIntVector(0, 1, 0),
	FIntVector(1, 1, 0),
	FIntVector(1, 1, 1),
	FIntVector(0, 1, 1));

ChunkFace ChunkFace::TopFace = ChunkFace(
	FIntVector(0, 0, 1),
	FIntVector(0, 1, 1),
	FIntVector(1, 1, 1),
	FIntVector(1, 0, 1));

ChunkFace ChunkFace::BottomFace = ChunkFace(
	FIntVector(0, 1, 0),
	FIntVector(0, 0, 0),
	FIntVector(1, 0, 0),
	FIntVector(1, 1, 0));
*/

FVector UChunkFace::GetStartVertexDown(double voxelSize) const
{
	return static_cast<FVector>(StartVertexDown) * voxelSize;
}

FVector UChunkFace::GetStartVertexUp(double voxelSize) const
{
	return static_cast<FVector>(StartVertexUp) * voxelSize;
}

FVector UChunkFace::GetEndVertexDown(double voxelSize) const
{
	return static_cast<FVector>(EndVertexDown) * voxelSize;
}

FVector UChunkFace::GetEndVertexUp(double voxelSize) const
{
	return static_cast<FVector>(EndVertexUp) * voxelSize;
}

UChunkFace UChunkFace::CreateFrontFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, UChunkFace());
}

UChunkFace UChunkFace::CreateBackFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, UChunkFace());
}

UChunkFace UChunkFace::CreateLeftFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel,UChunkFace());
}

UChunkFace UChunkFace::CreateRightFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, UChunkFace());
}

UChunkFace UChunkFace::CreateTopFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, UChunkFace());
}

UChunkFace UChunkFace::CreateBottomFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, UChunkFace());
}

int32 UChunkFace::GetVoxelId() const
{
	return Voxel.VoxelId;
}

bool UChunkFace::IsAxisStable(const FIntVector& mergeVertex, const FIntVector& otherMergeVertex,
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

/*
bool ChunkFace::MergeFace(const ChunkFace& otherFace, const EMergeMethod mergeMethod, const EUnstableAxis unstableAxis)
{
	switch (mergeMethod)
	{
	case EMergeMethod::Begin:
		if(IsAxisStable(BeginVertexDown, otherFace.BeginVertexDown, unstableAxis) &&
			IsAxisStable(BeginVertexUp, otherFace.BeginVertexUp, unstableAxis))
		{
			BeginVertexDown = otherFace.BeginVertexDown;
			BeginVertexUp = otherFace.BeginVertexUp;
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
		if(IsAxisStable(BeginVertexUp, otherFace.BeginVertexUp, unstableAxis) &&
			IsAxisStable(EndVertexUp, otherFace.EndVertexUp, unstableAxis))
		{
			EndVertexUp = otherFace.EndVertexUp;
			BeginVertexUp = otherFace.BeginVertexUp;
			return true;
		}
		return false;
		
	case EMergeMethod::Down:
		if(IsAxisStable(BeginVertexDown, otherFace.BeginVertexDown, unstableAxis) &&
			IsAxisStable(EndVertexDown, otherFace.EndVertexDown, unstableAxis))
		{
			BeginVertexDown = otherFace.BeginVertexDown;
			EndVertexDown = otherFace.EndVertexDown;
			return true;
		}
		return false;
		
	default:
		return false;
	}
}
*/

UChunkFace UChunkFace::CreateChunkFace(const FIntVector& InitialPosition, const FVoxel& voxel, UChunkFace face)
{
	face.Voxel = voxel;
	face.StartVertexDown += InitialPosition;
	face.StartVertexUp += InitialPosition;
	face.EndVertexDown += InitialPosition;
	face.EndVertexUp += InitialPosition;
	return face;
}

bool FrontNaiveMerge(UChunkFace& currentFace, const UChunkFace& otherFace)
{
	if(currentFace.Voxel == otherFace.Voxel &&
		currentFace.EndVertexDown.X == otherFace.EndVertexDown.X &&
		currentFace.EndVertexDown.Z == otherFace.EndVertexDown.Z &&

		
		currentFace.EndVertexUp.X == otherFace.EndVertexUp.X &&
		currentFace.EndVertexUp.Z == otherFace.EndVertexUp.Z)
	{
		currentFace.EndVertexDown = otherFace.EndVertexDown;
		currentFace.EndVertexUp = otherFace.EndVertexUp;
		return true;
	}
	return false;
}
