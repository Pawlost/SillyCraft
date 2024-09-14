#include "ChunkFace.h"

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