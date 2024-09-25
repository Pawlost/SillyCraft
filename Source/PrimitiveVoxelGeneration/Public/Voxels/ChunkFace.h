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
	FIntVector InitialPosition = FIntVector();
	
	static FChunkFace FrontFace;
	static FChunkFace BackFace;
	static FChunkFace LeftFace;
	static FChunkFace RightFace;
	static FChunkFace TopFace;
	static FChunkFace BottomFace;

	static FChunkFace CreateFrontFace(const FIntVector& InitialPosition, const FVoxel& voxel);
	static FChunkFace CreateBackFace(const FIntVector& InitialPosition, const FVoxel& voxel);
	static FChunkFace CreateLeftFace(const FIntVector& InitialPosition, const FVoxel& voxel);
	static FChunkFace CreateRightFace(const FIntVector& InitialPosition, const FVoxel& voxel);
	static FChunkFace CreateTopFace(const FIntVector& InitialPosition, const FVoxel& voxel);
	static FChunkFace CreateBottomFace(const FIntVector& InitialPosition, const FVoxel& voxel);

	bool IsAxisStable(const FChunkFace& otherFace) const;
	
	FChunkFace(){}
	
	FChunkFace(const FVector& beginVertexDown, const FVector& endVertexDown, const FVector& endVertexUp, const FVector& beginVertexUp)
	{
		BeginVertexDown = beginVertexDown;
		BeginVertexUp = beginVertexUp;
		EndVertexDown = endVertexDown;
		EndVertexUp = endVertexUp;
	}

private:
	UENUM(BlueprintType)
	enum class EUnstableAxis: uint8
	{
		X,
		Y,
		Z,
		Undefined
	};

	
	EUnstableAxis UnstableAxis = EUnstableAxis::Undefined;
	static FChunkFace CreateChunkFace(const FIntVector& InitialPosition, const FVoxel& voxel, FChunkFace face,  const EUnstableAxis& stableAxis);
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