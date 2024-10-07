#pragma once
#include "CoreMinimal.h"
#include "Voxel.h"

struct PRIMITIVEVOXELGENERATION_API FChunkFace
{
	FIntVector StartVertexDown;
	FIntVector StartVertexUp;
	FIntVector EndVertexDown;
	FIntVector EndVertexUp;

	FVector GetFinalStartVertexDown(const double& voxelSize) const;
	FVector GetFinalStartVertexUp(const double& voxelSize) const;
	FVector GetFinalEndVertexDown(const double& voxelSize) const;
	FVector GetFinalEndVertexUp(const double& voxelSize) const;
	
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
	
	FChunkFace(): StartVertexDown(), StartVertexUp(), EndVertexDown(), EndVertexUp()
	{
	}

	FChunkFace(const FIntVector& beginVertexDown, const FIntVector& endVertexDown, const FIntVector& endVertexUp, const FIntVector& beginVertexUp)
	{
		StartVertexDown = beginVertexDown;
		StartVertexUp = beginVertexUp;
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

	static bool MergeFaceEndY(FChunkFace& prevFace, const FChunkFace& newFace);
	static bool MergeFaceBeginY(FChunkFace& prevFace, const FChunkFace& newFace);
	static bool MergeFaceBeginX(FChunkFace& prevFace, const FChunkFace& newFace);
	static bool MergeFaceEndX(FChunkFace& prevFace, const FChunkFace& newFace);

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