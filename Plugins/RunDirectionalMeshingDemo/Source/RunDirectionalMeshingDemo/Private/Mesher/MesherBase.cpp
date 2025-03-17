// Fill out your copyright notice in the Description page of Project Settings.

#include "Mesher/MesherBase.h"

void UMesherBase::SetVoxelGenerator(const TObjectPtr<UVoxelGeneratorBase>& VoxelGeneratorBase)
{
	this->VoxelGenerator = VoxelGeneratorBase;
	VoxelGeneratorSet();
}
