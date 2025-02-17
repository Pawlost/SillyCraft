// // Fill out your copyright notice in the Description page of Project Settings.
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "Chunks/ChunkMesherBase.h"
// #include "Chunks/MeshingStructs/NaiveMeshingData.h"
// #include "Chunks/MeshingStructs/StaticNaiveMeshingData.h"
// #include "Voxels/ChunkFace.h"
// #include "DefaultChunkMesher.generated.h"
//
// struct FVoxel;
// /**
//  * 
//  */
// UCLASS()
// class PRIMITIVEVOXELGENERATION_API UDefaultChunkMesher : public UChunkMesherBase
// {
// 	GENERATED_BODY()
// 	
// public:
// 	virtual void AddToGrid(const TWeakObjectPtr<UChunkGridData> chunkGridData, FIntVector& chunkGridPos) override;
// 	virtual void GenerateMesh() override;
// 	virtual double GetHighestElevationAtPosition(double posX, double posY) override;
// 	
// private:
// 	struct VoxelIndexParams
// 	{
// 		bool isBorder;
// 		int32 forwardVoxelIndex;
// 		int32 previousVoxelIndex;
// 		int32 currentVoxelIndex;
// 		FIntVector borderChunkDirection;
// 	};
// 	
// 	bool IsBorderVoxelVisible(const VoxelIndexParams& faceData) const;
// 	bool IsVoxelVisible(const VoxelIndexParams& faceData);
//
// 	FNaiveMeshingData FrontFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::FrontFaceData);
// 	FNaiveMeshingData BackFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::BackFaceData);
// 	FNaiveMeshingData RightFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::RightFaceData);
// 	FNaiveMeshingData LeftFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::LeftFaceData);
// 	FNaiveMeshingData TopFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::TopFaceData);
// 	FNaiveMeshingData BottomFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::BottomFaceData);
//
// 	void UpdateAllFacesParams();
// 	void UpdateFaceParams(FNaiveMeshingData& face, FIntVector forwardVoxelIndexVector,
// 		FIntVector previousVoxelIndexVector, FIntVector chunkBorderIndexVector) const;
// 	
// 	void IncrementRun(int x, int y, int z, int32 axisVoxelIndex, bool isMinBorder, bool isMaxBorder,
// 	                         const FNaiveMeshingData& faceTemplate, const FNaiveMeshingData& reversedFaceTemplate,
// 	                         const TArray<TSharedPtr<TArray<FChunkFace>>>& faceContainer,
// 	                         const TArray<TSharedPtr<TArray<FChunkFace>>>& reversedFaceContainer);
// 	
// 	void AddFace(const FNaiveMeshingData& faceTemplate, bool isBorder, const int32& index,
// 		const FIntVector& position, const FVoxel& voxel, const int32& axisVoxelIndex,
// 		const TArray<TSharedPtr<TArray<FChunkFace>>>& faceContainer);
// 	void InitFaceContainers(TArray<TSharedPtr<TArray<FChunkFace>>>* faces);
// 	void FaceGeneration(const TArray<TSharedPtr<TArray<FChunkFace>>>* faces);
// 	void DirectionalGreedyMeshing(TArray<TSharedPtr<TArray<FChunkFace>>>* faces);
// 	void GenerateMeshFromFaces(const TArray<TSharedPtr<TArray<FChunkFace>>>* faces);
//
// 	static bool IsMinBorder(int x);
// 	bool IsMaxBorder(int x) const;
//
// 	struct FNormalsAndTangents
// 	{
// 		FVector3f Normal;
// 		FVector3f Tangent;
// 	};
// 	
// 	static const FNormalsAndTangents FaceNormalsAndTangents[6];
// };
