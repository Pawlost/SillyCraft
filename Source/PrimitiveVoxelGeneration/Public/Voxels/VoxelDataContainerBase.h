// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VoxelDataContainerBase.generated.h"


/**
 * 
 */
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API UVoxelDataContainerBase : public UObject
{
	GENERATED_BODY()

	void VoxelAt();
};
