// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkSettings.h"
#include "ProceduralMeshComponent.h"
#include "UObject/Object.h"
#include "ChunkBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API UChunkBase : public UObject
{
	GENERATED_BODY()
	
public:
	void SetChunkSettings(const TSharedPtr<FUChunkSettings> ChunkSettings);
	virtual void GenerateMesh(UProceduralMeshComponent* procMesh){}

protected:
	TSharedPtr<FUChunkSettings> ChunkSettings = nullptr;
};