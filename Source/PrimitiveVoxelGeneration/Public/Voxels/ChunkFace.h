#pragma once
#include "CoreMinimal.h"

struct PRIMITIVEVOXELGENERATION_API FChunkFace
{
	FVector BeginVertexDown;
	FVector BeginVertexUp;
	FVector EndVertexDown;
	FVector EndVertexUp;
	
	static FChunkFace FrontFace;
	static FChunkFace BackFace;
	static FChunkFace LeftFace;
	static FChunkFace RightFace;
	static FChunkFace TopFace;
	static FChunkFace BottomFace;

	FChunkFace(){}
	
	FChunkFace(const FVector& beginVertexDown, const FVector& beginVertexUp, const FVector& endVertexDown, const FVector& endVertexUp)
	{
		BeginVertexDown = beginVertexDown;
		BeginVertexUp = beginVertexUp;
		EndVertexDown = endVertexDown;
		EndVertexUp = endVertexUp;
	}
};

	
inline void operator+=(FChunkFace& ChunkFace, const FVector& Vector)
{
	ChunkFace.BeginVertexDown += Vector;
	ChunkFace.BeginVertexUp += Vector;
	ChunkFace.EndVertexDown += Vector;
	ChunkFace.EndVertexUp += Vector;
}

inline void operator-=(FChunkFace& ChunkFace, const FVector& Vector)
{
	ChunkFace.BeginVertexDown -= Vector;
	ChunkFace.BeginVertexUp -= Vector;
	ChunkFace.EndVertexDown -= Vector;
	ChunkFace.EndVertexUp -= Vector;
}

inline void operator*=(FChunkFace& ChunkFace, const int32& multiplier)
{
	ChunkFace.BeginVertexDown *= multiplier;
	ChunkFace.BeginVertexUp *= multiplier;
	ChunkFace.EndVertexDown *= multiplier;
	ChunkFace.EndVertexUp *= multiplier;
}