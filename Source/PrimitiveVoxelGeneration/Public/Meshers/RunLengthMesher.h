// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "MesherBase.h"
#include "Chunks/MeshingStructs/NaiveMeshingData.h"
#include "RunLengthMesher.generated.h"
//TODO: add forward declarations

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API URunLengthMesher : public UMesherBase
{
	GENERATED_BODY()
	
public:
	virtual void GenerateMesh(FChunkStruct& chunk) override;
	// virtual double GetHighestElevationAtPosition(double posX, double posY) override;
	virtual void GenerateVoxels(FChunkStruct& chunk) override;

	
	// Allows selecting a component class in Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TSubclassOf<UVoxelGeneratorBase> VoxelGeneratorClass = nullptr;
	
protected:
	virtual void BeginPlay() override;
private:
	struct FVoxelIndexParams
	{
		bool IsBorder;
		int32 ForwardVoxelIndex;
		int32 PreviousVoxelIndex;
		int32 CurrentVoxelIndex;
		FIntVector BorderChunkDirection;
	};

	struct FChunkFaceParams
	{
		TArray<TSharedRef<TArray<FChunkFace>>> Faces[FACE_COUNT];
		FChunkStruct ChunkStruct;
	};

	static bool IsBorderVoxelVisible(const FVoxelIndexParams& faceData, FChunkStruct& chunkStruct);
	static bool IsVoxelVisible(const FVoxelIndexParams& faceData, FChunkStruct& chunkStruct);
	
	FNaiveMeshingData FrontFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::FrontFaceData);
	FNaiveMeshingData BackFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::BackFaceData);
	FNaiveMeshingData RightFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::RightFaceData);
	FNaiveMeshingData LeftFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::LeftFaceData);
	FNaiveMeshingData TopFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::TopFaceData);
	FNaiveMeshingData BottomFaceTemplate = FNaiveMeshingData(FStaticNaiveMeshingData::BottomFaceData);

	void UpdateAllFacesParams();
	void UpdateFaceParams(FNaiveMeshingData& face, FIntVector forwardVoxelIndexVector,
		FIntVector previousVoxelIndexVector, FIntVector chunkBorderIndexVector) const;
	
	void IncrementRun(int x, int y, int z, int32 axisVoxelIndex, bool isMinBorder, bool isMaxBorder,
										const FNaiveMeshingData& faceTemplate, const FNaiveMeshingData& reversedFaceTemplate,
										const int32 faceContainerIndex,
										const int32 reversedFaceContainerIndex,
										FChunkFaceParams& chunkParams) const;

	static void AddFace(const FNaiveMeshingData& faceTemplate, bool isBorder,
	                    const int32& index, const FIntVector& position, const FVoxel& voxel, const int32& axisVoxelIndex,
	                    const TSharedPtr<TArray<FChunkFace>>& chunkFaces, FChunkStruct& chunkStruct);
	
	void InitFaceContainers(FChunkFaceParams& faceParams) const;
	void FaceGeneration(FChunkFaceParams& faceParams) const;
	static void DirectionalGreedyMeshing(const FChunkFaceParams& faceParams);
	void GenerateMeshFromFaces(const FChunkFaceParams& faceParams) const;

	static bool inline IsMinBorder(int x);
	bool inline IsMaxBorder(int x) const;

	struct FNormalsAndTangents
	{
		FVector3f Normal;
		FVector3f Tangent;
	};
	
	static const FNormalsAndTangents FaceNormalsAndTangents[6];
	
	UPROPERTY()
	UVoxelGeneratorBase* VoxelGenerator;
};