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

FChunkFace FChunkFace::CreateFrontFace(const FVector& InitialPosition)
{
	return FrontFace + InitialPosition;
}

FChunkFace FChunkFace::CreateBackFace(const FVector& InitialPosition)
{
	return BackFace + InitialPosition;
}

FChunkFace FChunkFace::CreateLeftFace(const FVector& InitialPosition)
{
	return LeftFace + InitialPosition;
}

FChunkFace FChunkFace::CreateRightFace(const FVector& InitialPosition)
{
	return RightFace + InitialPosition;
}

FChunkFace FChunkFace::CreateTopFace(const FVector& InitialPosition)
{
	return TopFace + InitialPosition;
}

FChunkFace FChunkFace::CreateBottomFace(const FVector& InitialPosition)
{
	return BottomFace + InitialPosition;
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