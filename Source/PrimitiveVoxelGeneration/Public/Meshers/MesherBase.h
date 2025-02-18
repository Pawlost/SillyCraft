// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Generators/VoxelGeneratorBase.h"
#include "MeshingStructs/ChunkFaceParams.h"
#include "Voxels/ChunkStruct.h"
#include "MesherBase.generated.h"
//TODO: add forward declarations

UCLASS(Abstract, Blueprintable)
class PRIMITIVEVOXELGENERATION_API UMesherBase : public UActorComponent, public IVoxelGenerator
{
	GENERATED_BODY()
	
public:
	virtual void GenerateVoxels(FChunkStruct& chunk) override {}

	virtual double GetChunkSize() override {return 0.0;}

	virtual void GenerateMesh(FChunkFaceParams& faceParams){}
};