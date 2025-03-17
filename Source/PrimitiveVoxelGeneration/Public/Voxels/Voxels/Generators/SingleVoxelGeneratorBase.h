// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxelGeneratorBase.h"
#include "SingleVoxelGeneratorBase.generated.h"

UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API USingleVoxelGeneratorBase : public UVoxelGeneratorBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxels")
	FDataTableRowHandle VoxelType;

protected:
	virtual void BeginPlay() override;
	FVoxel GetSingleVoxel() const;
	virtual TTuple<FName, FVoxelType> GetVoxelTypeById(const int32& voxelTypeIndex) const override;
	virtual FVoxel GetVoxelByName(const FName& voxelName) const override;
};
