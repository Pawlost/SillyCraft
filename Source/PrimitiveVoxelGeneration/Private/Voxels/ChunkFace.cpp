#include "Voxels/ChunkFace.h"

FChunkFace FChunkFace::FrontFace = FChunkFace(
	FVector(0.0, 0.0, 0.0),
	FVector(0.0, 1.0, 0.0),
	FVector(0.0, 1.0, 1.0),
	FVector(0.0, 0.0, 1.0));

FChunkFace FChunkFace::BackFace = FChunkFace(
	FVector(1.0, 1.0, 0.0),
	FVector(1.0, 0.0, 0.0),
	FVector(1.0, 0.0, 1.0),
	FVector(1.0, 1.0, 1.0));

FChunkFace FChunkFace::LeftFace = FChunkFace(
	FVector(1.0, 0.0, 0.0),
	FVector(0.0, 0.0, 0.0),
	FVector(0.0, 0.0, 1.0),
	FVector(1.0, 0.0, 1.0));

FChunkFace FChunkFace::RightFace = FChunkFace(
	FVector(0.0, 1.0, 0.0),
	FVector(1.0, 1.0, 0.0),
	FVector(1.0, 1.0, 1.0),
	FVector(0.0, 1.0, 1.0));

FChunkFace FChunkFace::TopFace = FChunkFace(
	FVector(0.0, 0.0, 1.0),
	FVector(0.0, 1.0, 1.0),
	FVector(1.0, 1.0, 1.0),
	FVector(1.0, 0.0, 1.0));

FChunkFace FChunkFace::BottomFace = FChunkFace(
	FVector(0.0, 1.0, 0.0),
	FVector(0.0, 0.0, 0.0),
	FVector(1.0, 0.0, 0.0),
	FVector(1.0, 1.0, 0.0));

FChunkFace FChunkFace::CreateFrontFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel, FrontFace, EUnstableAxis::X);
}

FChunkFace FChunkFace::CreateBackFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel,BackFace, EUnstableAxis::X);
}

FChunkFace FChunkFace::CreateLeftFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel,LeftFace, EUnstableAxis::Y);
}

FChunkFace FChunkFace::CreateRightFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel,RightFace, EUnstableAxis::Y);
}

FChunkFace FChunkFace::CreateTopFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel,TopFace, EUnstableAxis::Y);
}

FChunkFace FChunkFace::CreateBottomFace(const FIntVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(InitialPosition, voxel,BottomFace, EUnstableAxis::Y);
}

bool FChunkFace::IsAxisStable(const FChunkFace& otherFace) const
{
	switch (UnstableAxis)
	{
	case EUnstableAxis::X:
		return otherFace.InitialPosition.Z == InitialPosition.Z && otherFace.InitialPosition.Y == InitialPosition.Y;
	case EUnstableAxis::Y:
		return otherFace.InitialPosition.Z == InitialPosition.Z && otherFace.InitialPosition.X == InitialPosition.X;
	case EUnstableAxis::Z:
		return otherFace.InitialPosition.Y == InitialPosition.Y && otherFace.InitialPosition.X == InitialPosition.X;
	default:
		return false;
	}
}

FChunkFace FChunkFace::CreateChunkFace(const FIntVector& InitialPosition, const FVoxel& voxel, FChunkFace face,
	const EUnstableAxis& stableAxis)
{
	face.Voxel = voxel;
	face.InitialPosition = InitialPosition;
	face.UnstableAxis = stableAxis;
	return face + static_cast<FVector>(InitialPosition);
}

void operator+=(FChunkFace& ChunkFace, const FVector& Vector)
{
	ChunkFace.BeginVertexDown += Vector;
	ChunkFace.BeginVertexUp += Vector;
	ChunkFace.EndVertexDown += Vector;
	ChunkFace.EndVertexUp += Vector;
}

FChunkFace operator+(FChunkFace ChunkFace, const FVector& Vector)
{
	ChunkFace += Vector;
	return ChunkFace;
}

void operator-=(FChunkFace& ChunkFace, const FVector& Vector)
{
		ChunkFace.BeginVertexDown -= Vector;
		ChunkFace.BeginVertexUp -= Vector;
		ChunkFace.EndVertexDown -= Vector;
		ChunkFace.EndVertexUp -= Vector;
}

FChunkFace operator-(FChunkFace ChunkFace, const FVector& Vector)
{
	ChunkFace -= Vector;
	return ChunkFace;
}