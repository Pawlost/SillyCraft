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
	struct NaiveMeshingData
	{
		bool isBorder;
		int32 forwardVoxelIndex;
		int32 neighborChunkVoxelIndex;
		FIntVector neighborChunkDistance;
		FChunkFace face;
		int32 previousVoxelIndex;
		TFunctionRef<bool(FChunkFace& prevFace, const FChunkFace& newFace)> mergeFaces;
	};
	
	bool ChunkCull(const NaiveMeshingData& faceData) const;
	bool VoxelCull(const NaiveMeshingData& faceData);
	
	void GenerateFacesInAxis(int x, int y, int z,
					NaiveMeshingData faceData, NaiveMeshingData reversedFaceData,
					const TSharedRef<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>& faceContainer,
					const TSharedRef<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>& reversedFaceContainer);
	
	static void GreedyMeshing(int32 voxelId, TMap<int32, TSharedPtr<TArray<FChunkFace>>>& faces,
	                         TFunctionRef<bool(FChunkFace& prevFace, const FChunkFace& newFace)> mergeFaces);

	void CreateFace(NaiveMeshingData& faceData, const int32& index, const FIntVector& position, const FVoxel& voxel, const TSharedRef<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>& faceContainer);
	void InitFaceContainers(TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces);
	void FaceGeneration(const TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces);
	void GreedyMeshAllFaces(const TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces);
	void GenerateMeshFromFaces(const TSharedPtr<TMap<int32, TSharedPtr<TArray<FChunkFace>>>>* faces);

	bool inline IsMinBorder(int x);
	bool inline IsMaxBorder(int x) const;
	
	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> Noise;

	UPROPERTY()
	TArray<FVoxel> Voxels;

	UPROPERTY()
	TSet<int32> voxelIdsInMesh;

	TSharedPtr<FChunkSettings> ChunkSettings;
	int32 ChunkLenght;
};
