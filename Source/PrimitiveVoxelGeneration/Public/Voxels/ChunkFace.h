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

	static FChunkFace CreateFrontFace(const FVector& InitialPosition);
	static FChunkFace CreateBackFace(const FVector& InitialPosition);
	static FChunkFace CreateLeftFace(const FVector& InitialPosition);
	static FChunkFace CreateRightFace(const FVector& InitialPosition);
	static FChunkFace CreateTopFace(const FVector& InitialPosition);
	static FChunkFace CreateBottomFace(const FVector& InitialPosition);
	
	FChunkFace(){}
	
	FChunkFace(const FVector& beginVertexDown, const FVector& beginVertexUp, const FVector& endVertexDown, const FVector& endVertexUp)
	{
		BeginVertexDown = beginVertexDown;
		BeginVertexUp = beginVertexUp;
		EndVertexDown = endVertexDown;
		EndVertexUp = endVertexUp;
	}
};

	
inline void operator+=(FChunkFace& ChunkFace, const FVector& Vector);

inline FChunkFace operator+(FChunkFace ChunkFace, const FVector& Vector);

inline void operator-=(FChunkFace& ChunkFace, const FVector& Vector);

inline FChunkFace operator-(FChunkFace ChunkFace, const FVector& Vector);

inline void operator*=(FChunkFace& ChunkFace, const int32& Multiplier)
{
	ChunkFace.BeginVertexDown *= Multiplier;
	ChunkFace.BeginVertexUp *= Multiplier;
	ChunkFace.EndVertexDown *= Multiplier;
	ChunkFace.EndVertexUp *= Multiplier;
}