﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkBase.h"
#include "ChunkGridData.h"
#include "FastNoiseWrapper.h"
#include "MeshingStructs/NaiveMeshingData.h"
#include "MeshingStructs/StaticNaiveMeshingData.h"
#include "Voxels/ChunkFace.h"
#include "DefaultChunk.generated.h"

#define FACE_COUNT 6

#define FRONT_FACE_INDEX 0
#define BACK_FACE_INDEX 1
#define RIGHT_FACE_INDEX 2
#define LEFT_FACE_INDEX 3
#define BOTTOM_FACE_INDEX 4
#define TOP_FACE_INDEX 5

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
	struct VoxelIndexParams
	{
		bool isBorder;
		int32 forwardVoxelIndex;
		int32 previousVoxelIndex;
		int32 currentVoxelIndex;
		FIntVector borderChunkDirection;
	};
	
	bool IsBorderVoxelVisible(const VoxelIndexParams& faceData) const;
	bool IsVoxelVisible(const VoxelIndexParams& faceData);

	FNaiveMeshingData FrontFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::FrontFaceData);
	FNaiveMeshingData BackFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::BackFaceData);
	FNaiveMeshingData RightFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::RightFaceData);
	FNaiveMeshingData LeftFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::LeftFaceData);
	FNaiveMeshingData TopFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::TopFaceData);
	FNaiveMeshingData BottomFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::BottomFaceData);

	void UpdateAllFacesParams();
	void UpdateFaceParams(FNaiveMeshingData& face, FIntVector forwardVoxelIndexVector,
		FIntVector previousVoxelIndexVector, FIntVector chunkBorderIndexVector) const;
	
	void GenerateFacesInAxis(int x, int y, int z, int32 axisVoxelIndex, bool isMinBorder, bool isMaxBorder,
	                         const FNaiveMeshingData& faceTemplate, const FNaiveMeshingData& reversedFaceTemplate,
	                         const TArray<TSharedPtr<TArray<FChunkFace>>>& faceContainer,
	                         const TArray<TSharedPtr<TArray<FChunkFace>>>& reversedFaceContainer);
	
	void CreateFace(const FNaiveMeshingData& faceTemplate, bool isBorder, const int32& index,
		const FIntVector& position, const FVoxel& voxel, const int32& axisVoxelIndex,
		const TArray<TSharedPtr<TArray<FChunkFace>>>& faceContainer);
	void InitFaceContainers(TArray<TSharedPtr<TArray<FChunkFace>>>* faces);
	void FaceGeneration(const TArray<TSharedPtr<TArray<FChunkFace>>>* faces);
	void GreedyMeshing(TArray<TSharedPtr<TArray<FChunkFace>>>* faces);
	void GenerateMeshFromFaces(const TArray<TSharedPtr<TArray<FChunkFace>>>* faces);

	static bool inline IsMinBorder(int x);
	bool inline IsMaxBorder(int x) const;

	struct FNormalsAndTangents
	{
		FVector3f Normal;
		FVector3f Tangent;
	};
	
	static const FNormalsAndTangents FaceNormalsAndTangents[6];
	
	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> Noise;

	UPROPERTY()
	TArray<FVoxel> Voxels;

	UPROPERTY()
	TMap<int32, int16> voxelIdsInMesh;

	TSharedPtr<FChunkSettings> ChunkSettings;
	int32 ChunkLenght;
};
