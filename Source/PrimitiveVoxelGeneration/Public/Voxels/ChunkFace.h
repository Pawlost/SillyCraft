#pragma once
#include "CoreMinimal.h"
#include "Voxel.h"

struct PRIMITIVEVOXELGENERATION_API FChunkFace
{
	FVector BeginVertexDown;
	FVector BeginVertexUp;
	FVector EndVertexDown;
	FVector EndVertexUp;
	FVoxel Voxel;
	
	static FChunkFace FrontFace;
	static FChunkFace BackFace;
	static FChunkFace LeftFace;
	static FChunkFace RightFace;
	static FChunkFace TopFace;
	static FChunkFace BottomFace;

	static FChunkFace CreateFrontFace(const FVector& InitialPosition, const FVoxel& Voxel);
	static FChunkFace CreateBackFace(const FVector& InitialPosition, const FVoxel& Voxel);
	static FChunkFace CreateLeftFace(const FVector& InitialPosition, const FVoxel& Voxel);
	static FChunkFace CreateRightFace(const FVector& InitialPosition, const FVoxel& Voxel);
	static FChunkFace CreateTopFace(const FVector& InitialPosition, const FVoxel& Voxel);
	static FChunkFace CreateBottomFace(const FVector& InitialPosition, const FVoxel& Voxel);
	
	FChunkFace(){}
	
	FChunkFace(const FVector& beginVertexDown, const FVector& beginVertexUp, const FVector& endVertexDown, const FVector& endVertexUp)
	{
		BeginVertexDown = beginVertexDown;
		BeginVertexUp = beginVertexUp;
		EndVertexDown = endVertexDown;
		EndVertexUp = endVertexUp;
	}

private:
	static FChunkFace CreateChunkFace(FChunkFace face, const FVector& InitialPosition, const FVoxel& Voxel);
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