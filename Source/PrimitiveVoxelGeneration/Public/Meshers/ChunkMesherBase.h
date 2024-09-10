// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "UObject/Object.h"
#include "ChunkMesherBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API UChunkMesherBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(UProceduralMeshComponent* actor){}
};