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

	static bool MergeFaceEndY(FChunkFace& prevFace, const FChunkFace& newFace);
	static bool MergeFaceBeginY(FChunkFace& prevFace, const FChunkFace& newFace);
	static bool MergeFaceBeginX(FChunkFace& prevFace, const FChunkFace& newFace);
	static bool MergeFaceEndX(FChunkFace& prevFace, const FChunkFace& newFace);
	
	static bool MergeFaceDownZ(FChunkFace& prevFace, const FChunkFace& newFace);
	static bool MergeFaceDownX(FChunkFace& prevFace, const FChunkFace& newFace);
	
private:
	static FChunkFace CreateChunkFace(const FIntVector& InitialPosition, const FVoxel& voxel, FChunkFace face);
};

inline void operator+=(FChunkFace& ChunkFace, const FIntVector& Vector);

inline void operator-=(FChunkFace& ChunkFace, const FIntVector& Vector);

inline FChunkFace operator-(FChunkFace ChunkFace, const FIntVector& Vector);