// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkBase.h"
#include "DefaultChunk.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVEVOXELGENERATION_API UDefaultChunk : public UChunkBase
{
	GENERATED_BODY()

	public:
		UDefaultChunk(){}
		virtual void GenerateMesh(UProceduralMeshComponent* procMesh) override;
};
