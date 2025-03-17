#pragma once
#include "CoreMinimal.h"
#include "MesherBase.h"
#include "Mesh/RealtimeMeshDataStream.h"
#include "MeshingUtils/MeshingDirections.h"
#include "RunDirectionalMesher.generated.h"

struct FChunkParams;
enum class EFaceDirection;

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RUNDIRECTIONALMESHINGDEMO_API URunDirectionalMesher : public UMesherBase
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(FMesherVariables& faceParams) override;
	
private:
	struct FVoxelIndexParams
	{
		bool IsBorder;
		int32 ForwardVoxelIndex;
		int32 PreviousVoxelIndex;
		int32 CurrentVoxelIndex;
		FVoxel CurrentVoxel;
		EFaceDirection FaceDirection;
	};

	static bool IsBorderVoxelVisible(const FVoxelIndexParams& FaceData, const FChunkParams& ChunkStruct);
	static bool IsVoxelVisible(const FVoxelIndexParams& FaceData, const FChunkParams& ChunkStruct);

	void IncrementRun(int X, int Y, int Z, int32 AxisVoxelIndex, bool bIsMinBorder, bool bIsMaxBorder,
	                  const FMeshingDirections& FaceTemplate, const FMeshingDirections& ReversedFaceTemplate,
	                  FMesherVariables& MesherVariables) const;

	static void AddFace(const FMeshingDirections& FaceTemplate, bool bIsBorder,
	                    const int32& Index, const FIntVector& Position, const FVoxel& Voxel,
	                    const int32& AxisVoxelIndex,
	                    const TSharedPtr<TArray<FChunkFace>>& ChunkFaces, const FChunkParams& ChunkParams);

	static void InitFaceContainers(FMesherVariables& MesherVariables);
	void FaceGeneration(FMesherVariables& MesherVariables) const;
	static void DirectionalGreedyMeshing(const FMesherVariables& MesherVariables);
	void GenerateMeshFromFaces(const FMesherVariables& MesherVariables) const;
	
	void GenerateActorMesh(const TMap<uint32, uint16>& LocalVoxelTable,
	                       const RealtimeMesh::FRealtimeMeshStreamSet& StreamSet,
	                       const TSharedPtr<FChunkParams>& ChunkParams) const;
};
