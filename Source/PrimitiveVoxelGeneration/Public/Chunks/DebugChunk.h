// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkBase.h"
#include "DebugChunk.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PRIMITIVEVOXELGENERATION_API UDebugChunk : public UChunkBase
{
	GENERATED_BODY()
public:
	UDebugChunk(){}

	virtual void GenerateMesh(UProceduralMeshComponent* procMesh) override;
};
