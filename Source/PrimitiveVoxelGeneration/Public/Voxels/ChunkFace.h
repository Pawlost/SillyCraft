#pragma once

#include "Voxel.h"
#include "CoreMinimal.h"
#include "ChunkFace.generated.h"

UENUM(BlueprintType)
enum class EMergeMethod: uint8
{
	Up,
	Down,
	End,
	Begin
};

UENUM(BlueprintType)
enum class EUnstableAxis: uint8
{
	X,
	Y,
	Z,
	Undefined
};

UCLASS()
class PRIMITIVEVOXELGENERATION_API UChunkFace : public UObject
{
GENERATED_BODY()
	
public:
	FVector GetStartVertexDown(double voxelSize = 1.0) const;
	FVector GetStartVertexUp(double voxelSize = 1.0) const;
	FVector GetEndVertexDown(double voxelSize = 1.0) const;
	FVector GetEndVertexUp(double voxelSize = 1.0) const;

	static UChunkFace CreateFrontFace(const FIntVector& InitialPosition, const FVoxel& voxel);
	static UChunkFace CreateBackFace(const FIntVector& InitialPosition, const FVoxel& voxel);
	static UChunkFace CreateLeftFace(const FIntVector& InitialPosition, const FVoxel& voxel);
	static UChunkFace CreateRightFace(const FIntVector& InitialPosition, const FVoxel& voxel);
	static UChunkFace CreateTopFace(const FIntVector& InitialPosition, const FVoxel& voxel);
	static UChunkFace CreateBottomFace(const FIntVector& InitialPosition, const FVoxel& voxel);

	int32 GetVoxelId() const;
	virtual bool NaiveFaceMerge(const UChunkFace& otherFace){return false;};
	virtual bool GreedyFaceMerge(const UChunkFace& otherFace){return false;};

protected:
	FIntVector StartVertexDown;
	FIntVector StartVertexUp;
	FIntVector EndVertexDown;
	FIntVector EndVertexUp;
	FVoxel Voxel;

	friend bool FrontNaiveMerge(UChunkFace& currentFace, const UChunkFace& otherFace);
	
	UChunkFace(): StartVertexDown(), StartVertexUp(), EndVertexDown(), EndVertexUp()
	{
	}
	
	UChunkFace(const FIntVector& startVertexDown, const FIntVector& endVertexDown, const FIntVector& endVertexUp, const FIntVector& startVertexUp,
		const FVoxel& voxel, const FIntVector& InitialPosition)
	{
		StartVertexDown = startVertexDown + InitialPosition;
		StartVertexUp = startVertexUp + InitialPosition;
		EndVertexDown = endVertexDown + InitialPosition;
		EndVertexUp = endVertexUp + InitialPosition;
		Voxel = voxel;
	}


	//bool MergeFace(const ChunkFace& otherFace, const EMergeMethod mergeMethod, const EUnstableAxis unstableAxis);

private:
	// returns true if merge was succesful
	static bool IsAxisStable(const FIntVector& mergeVertex, const FIntVector& otherMergeVertex,
		const EUnstableAxis unstableAxis);
	static UChunkFace CreateChunkFace(const FIntVector& InitialPosition, const FVoxel& voxel, UChunkFace face);
};