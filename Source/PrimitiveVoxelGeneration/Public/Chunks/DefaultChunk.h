// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkBase.h"
#include "ChunkGridData.h"
#include "FastNoiseWrapper.h"
#include "Voxels/ChunkFace.h"
#include "DefaultChunk.generated.h"

#define FACE_COUNT 6

struct FVoxel;
/**
 * 
 */
UCLASS()
class PRIMITIVEVOXELGENERATION_API UDefaultChunk : public UChunkBase
{
public:
	virtual void AddToGrid(const TWeakObjectPtr<UChunkGridData> chunkGridData, FIntVector& chunkGridPos) override;

private:
	GENERATED_BODY()

public:
	UDefaultChunk();
	virtual void GenerateVoxels() override;
	virtual void GenerateMesh() override;
	virtual FVoxel VoxelAt(int32 index) override;
	virtual double GetHighestElevationAtPosition(double posX, double posY) override;
	
private:
	void AddNaiveMeshedFace(const FChunkFace& face,
	                        TMap<int32, TSharedPtr<TArray<FChunkFace>>>& faces, int32 previousVoxelDirection,
	                        FChunkFace::EMergeMethod mergeMethod, FChunkFace::EUnstableAxis unstableAxis);
	
	bool ChunkCull(int32 chunkIndex, const FIntVector& neighborChunkDistance) const;
	bool VoxelCull(int32 forwardVoxelIndex);

	bool CrossChunkCullInNegativeDirection(int min, int32 forwardVoxelIndex, int32 chunkIndex, FIntVector neighborChunkDistance);
	bool CrossChunkCullInPositiveDirection(int max, int32 forwardVoxelIndex, int32 chunkIndex, FIntVector neighborChunkDistance);

	static void GreedyMeshing(TMap<int32, TSharedPtr<TArray<FChunkFace>>>& faces,
	                          FChunkFace::EMergeMethod mergeMethod, FChunkFace::EUnstableAxis unstableAxis);

	void InitFaces(TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces);
	void FaceGeneration(const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces);
	static void GreedyMeshFaces(const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces);
	void GenerateMeshFromFaces(const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces);

	void GenerateFacesInXAxis(int x, int y, int z, const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces);
	void GenerateFacesInYAxis(int x, int y, int z, const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces);
	void GenerateFacesInZAxis(int x, int y, int z, const TUniquePtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces);
	
	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> Noise;

	UPROPERTY()
	TArray<FVoxel> Voxels;

	UPROPERTY()
	TSet<int32> voxelIdsInMesh;

	TSharedPtr<FChunkSettings> ChunkSettings;
};
