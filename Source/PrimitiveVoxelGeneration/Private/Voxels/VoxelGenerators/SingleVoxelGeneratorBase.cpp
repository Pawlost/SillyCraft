﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Voxels/VoxelGenerators/SingleVoxelGeneratorBase.h"

void USingleVoxelGeneratorBase::BeginPlay()
{
	checkf(!VoxelType.IsNull(), TEXT("Voxel Type must by set"))
	Super::BeginPlay();
}

FVoxel USingleVoxelGeneratorBase::GetSingleVoxel() const
{
	return GetVoxelByName(VoxelType.RowName);
}

TTuple<FName, FVoxelType> USingleVoxelGeneratorBase::GetVoxelTypeById(const int32& voxelTypeIndex) const
{

	auto voxelTable = VoxelType.DataTable;
	auto rowName = voxelTable->GetRowNames()[voxelTypeIndex];

	return TTuple<FName, FVoxelType>(rowName, *voxelTable->FindRow<FVoxelType>(rowName, ""));
}

FVoxel USingleVoxelGeneratorBase::GetVoxelByName(const FName& voxelName) const
{
	TArray<FName> RowNames = VoxelType.DataTable->GetRowNames();
	for (int32 Index = 0; Index < RowNames.Num(); Index++)
	{
		if (RowNames[Index] == voxelName)
		{
			auto voxelType = VoxelType.DataTable->FindRow<FVoxelType>(VoxelType.RowName, "");
			return FVoxel(Index, voxelType->bIsTransparent);
		}
	}
	return FVoxel();
}
