// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkMesherBase.h"
#include "DefaultChunkMesher.generated.h"

/**
 * 
 */
UCLASS()
class PRIMITIVEVOXELGENERATION_API UDefaultChunkMesher : public UChunkMesherBase
{
	GENERATED_BODY()

	public:
		UDefaultChunkMesher(){}
		virtual void GenerateMesh(UProceduralMeshComponent* procMesh) override;
};
