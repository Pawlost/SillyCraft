// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VoxelContainerBase.generated.h"


/**
 * 
 */
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API UVoxelContainerBase : public UObject
{
	GENERATED_BODY()

	void VoxelAt();
};
