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

FChunkFace FChunkFace::CreateFrontFace(const FVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(FrontFace, InitialPosition, voxel);
}

FChunkFace FChunkFace::CreateBackFace(const FVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(BackFace, InitialPosition, voxel);
}

FChunkFace FChunkFace::CreateLeftFace(const FVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(LeftFace, InitialPosition, voxel);
}

FChunkFace FChunkFace::CreateRightFace(const FVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(RightFace, InitialPosition, voxel);
}

FChunkFace FChunkFace::CreateTopFace(const FVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(TopFace, InitialPosition, voxel);
}

FChunkFace FChunkFace::CreateBottomFace(const FVector& InitialPosition, const FVoxel& voxel)
{
	return CreateChunkFace(BottomFace, InitialPosition, voxel);
}

FChunkFace FChunkFace::CreateChunkFace(FChunkFace face, const FVector& InitialPosition, const FVoxel& voxel)
{
	face.Voxel = voxel;
	return face + InitialPosition;
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