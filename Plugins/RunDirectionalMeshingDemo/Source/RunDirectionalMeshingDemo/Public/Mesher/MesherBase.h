// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Voxel/Generators/VoxelGeneratorBase.h"
#include "MesherBase.generated.h"

struct FMesherVariables;

/**
 * Base for components used to convert voxel models to mesh
 */
UCLASS(Abstract, Blueprintable)
class RUNDIRECTIONALMESHINGDEMO_API UMesherBase : public UActorComponent
{
	GENERATED_BODY()

public:
	void SetVoxelGenerator(const TObjectPtr<UVoxelGeneratorBase>& VoxelGeneratorBase);

	virtual void GenerateMesh(FMesherVariables& faceParams)
	{
	}

protected:
	virtual void VoxelGeneratorSet()
	{
	}

	UPROPERTY()
	TObjectPtr<UVoxelGeneratorBase> VoxelGenerator;
};
