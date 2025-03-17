#pragma once
#include "CoreMinimal.h"
#include "RunDirectionalMeshingDemo/Public/Voxel/Voxel.h"

/**
 * Struct representing single voxel face. Inside helper struct not intended to be used with Unreal Engine.
 */
struct RUNDIRECTIONALMESHINGDEMO_API FChunkFace
{
	FVoxel Voxel;
	FIntVector StartVertexDown;
	FIntVector EndVertexDown;
	FIntVector EndVertexUp;
	FIntVector StartVertexUp;

	// Create something like singletons
	static FChunkFace FrontFace;
	static FChunkFace BackFace;
	static FChunkFace LeftFace;
	static FChunkFace RightFace;
	static FChunkFace TopFace;
	static FChunkFace BottomFace;

	FChunkFace() : Voxel(FVoxel()), StartVertexDown(), EndVertexDown(), EndVertexUp(), StartVertexUp()
	{
	}

	FChunkFace(const FVoxel& Voxel, const FIntVector& StartVertexDown, const FIntVector& EndVertexDown,
	           const FIntVector& EndVertexUp, const FIntVector& StartVertexUp) : Voxel(Voxel),
		StartVertexDown(StartVertexDown),
		EndVertexDown(EndVertexDown), EndVertexUp(EndVertexUp), StartVertexUp(StartVertexUp)
	{
	}

	FChunkFace(const FIntVector& StartVertexDown, const FIntVector& EndVertexDown,
	           const FIntVector& EndVertexUp, const FIntVector& StartVertexUp) : StartVertexDown(StartVertexDown),
		EndVertexDown(EndVertexDown), EndVertexUp(EndVertexUp), StartVertexUp(StartVertexUp)
	{
	}

	static FChunkFace CreateFrontFace(const FIntVector& InitialPosition, const FVoxel& Voxel);
	static FChunkFace CreateBackFace(const FIntVector& InitialPosition, const FVoxel& Voxel);
	static FChunkFace CreateLeftFace(const FIntVector& InitialPosition, const FVoxel& Voxel);
	static FChunkFace CreateRightFace(const FIntVector& InitialPosition, const FVoxel& Voxel);
	static FChunkFace CreateTopFace(const FIntVector& InitialPosition, const FVoxel& Voxel);
	static FChunkFace CreateBottomFace(const FIntVector& InitialPosition, const FVoxel& Voxel);

	static bool MergeFaceEnd(FChunkFace& PrevFace, const FChunkFace& NewFace);
	static bool MergeFaceStart(FChunkFace& PrevFace, const FChunkFace& NewFace);
	static bool MergeFaceUp(FChunkFace& PrevFace, const FChunkFace& NewFace);

	FVector3f GetFinalStartVertexDown(const double& VoxelSize) const;
	FVector3f GetFinalStartVertexUp(const double& VoxelSize) const;
	FVector3f GetFinalEndVertexDown(const double& VoxelSize) const;
	FVector3f GetFinalEndVertexUp(const double& VoxelSize) const;

private:
	static FChunkFace CreateChunkFace(const FIntVector& InitialPosition, const FVoxel& Voxel, FChunkFace Face);
};
