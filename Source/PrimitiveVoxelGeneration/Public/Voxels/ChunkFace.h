#pragma once
#include "CoreMinimal.h"
#include "Voxel.h"

struct PRIMITIVEVOXELGENERATION_API FChunkFace
{
	FIntVector BeginVertexDown;
	FIntVector BeginVertexUp;
	FIntVector EndVertexDown;
	FIntVector EndVertexUp;
	bool IsMark = false;
	FVoxel Voxel = FVoxel();
	
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

	UENUM(BlueprintType)
	enum class EMergeMethod: uint8
	{
		Up,
		Down,
		End,
		Begin
	};
	
	FChunkFace(){}
	
	FChunkFace(const FIntVector& beginVertexDown, const FIntVector& endVertexDown, const FIntVector& endVertexUp, const FIntVector& beginVertexUp)
	{
		BeginVertexDown = beginVertexDown;
		BeginVertexUp = beginVertexUp;
		EndVertexDown = endVertexDown;
		EndVertexUp = endVertexUp;
	}

	UENUM(BlueprintType)
	enum class EUnstableAxis: uint8
	{
		X,
		Y,
		Z,
		Undefined
	};

	bool MergeFace(const FChunkFace& otherFace, const EMergeMethod mergeMethod, const EUnstableAxis unstableAxis);
	
private:
	// returns true if merge was succesful
	static bool IsAxisStable(const FIntVector& mergeVertex, const FIntVector& otherMergeVertex,
		const EUnstableAxis unstableAxis);
	static FChunkFace CreateChunkFace(const FIntVector& InitialPosition, const FVoxel& voxel, FChunkFace face);
};

	
inline void operator+=(FChunkFace& ChunkFace, const FIntVector& Vector);

inline void operator-=(FChunkFace& ChunkFace, const FIntVector& Vector);

inline FChunkFace operator-(FChunkFace ChunkFace, const FIntVector& Vector);

inline void operator*=(FChunkFace& ChunkFace, const int32& Multiplier)
{
	ChunkFace.BeginVertexDown *= Multiplier;
	ChunkFace.BeginVertexUp *= Multiplier;
	ChunkFace.EndVertexDown *= Multiplier;
	ChunkFace.EndVertexUp *= Multiplier;
}