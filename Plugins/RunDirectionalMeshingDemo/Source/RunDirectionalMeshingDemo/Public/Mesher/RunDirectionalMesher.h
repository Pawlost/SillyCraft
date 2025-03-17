// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "MesherBase.h"
#include "Mesh/RealtimeMeshDataStream.h"
#include "MeshingStructs/MesherVariables.h"
#include "MeshingStructs/NaiveMeshingData.h"
#include "Voxel/Voxel.h"
#include "RunDirectionalMesher.generated.h"
//TODO: add forward declarations

UCLASS(ClassGroup=(Meshers), meta=(BlueprintSpawnableComponent), Blueprintable)
class RUNDIRECTIONALMESHINGDEMO_API URunDirectionalMesher : public UMesherBase
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(FMesherVariables& faceParams) override;

protected:
	virtual void VoxelGeneratorSet() override;

private:
	struct FVoxelIndexParams
	{
		bool IsBorder;
		int32 ForwardVoxelIndex;
		int32 PreviousVoxelIndex;
		int32 CurrentVoxelIndex;
		FVoxel currentVoxel;
		EFaceDirection FaceDirection;
	};

	static bool IsBorderVoxelVisible(const FVoxelIndexParams& faceData, const FChunkParams& chunkStruct);
	static bool IsVoxelVisible(const FVoxelIndexParams& faceData, const FChunkParams& chunkStruct);

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
	                  FMesherVariables& chunkParams) const;

	static void AddFace(const FNaiveMeshingData& faceTemplate, bool isBorder,
	                    const int32& index, const FIntVector& position, const FVoxel& voxel,
	                    const int32& axisVoxelIndex,
	                    const TSharedPtr<TArray<FChunkFace>>& chunkFaces, const FChunkParams& chunkParams);

	void InitFaceContainers(FMesherVariables& faceParams) const;
	void FaceGeneration(FMesherVariables& faceParams) const;
	static void DirectionalGreedyMeshing(const FMesherVariables& faceParams);
	void GenerateMeshFromFaces(const FMesherVariables& faceParams) const;

	static bool inline IsMinBorder(int x);
	bool inline IsMaxBorder(int x) const;

	struct FNormalsAndTangents
	{
		FVector3f Normal;
		FVector3f Tangent;
	};

	static const FNormalsAndTangents FaceNormalsAndTangents[FACE_SIDE_COUNT];

	void GenerateActorMesh(const TMap<uint32, uint16>& voxelIdsInMesh,
	                       const RealtimeMesh::FRealtimeMeshStreamSet& StreamSet,
	                       const TSharedPtr<FChunkParams>& ChunkParams) const;
};
