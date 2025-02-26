// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "MeshingStructs/ChunkFaceParams.h"
#include "Voxels/ChunkStruct.h"
#include "Voxels/Interfaces/IVoxelGenerator.h"
#include "MesherBase.generated.h"
//TODO: add forward declarations

UCLASS(Abstract, Blueprintable)
class PRIMITIVEVOXELGENERATION_API UMesherBase : public UActorComponent, public IVoxelGenerator
{
	GENERATED_BODY()

public:
	virtual void GenerateVoxels(TSharedPtr<FChunkStruct>& chunk) override
	{
	}

	virtual double GetChunkSize() override { return 0.0; }


	virtual double GetVoxelSize() override { return 0.0; }

	virtual void GenerateMesh(FChunkFaceParams& faceParams)
	{
	}
	virtual bool ChangeVoxelIdInChunk(const TSharedPtr<FChunkStruct>& chunk, const FIntVector& voxelPosition, const FVoxel& voxelId)
	{
		return false;
	}
	virtual double GetHighestElevationAtLocation(const FVector& location) override { return 0.0; }
};
