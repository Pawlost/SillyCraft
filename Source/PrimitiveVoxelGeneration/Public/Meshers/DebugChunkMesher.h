// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkMesherBase.h"
#include "DebugChunkMesher.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class PRIMITIVEVOXELGENERATION_API UDebugChunkMesher : public UChunkMesherBase
{
	GENERATED_BODY()
public:
	UDebugChunkMesher(){}
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMesh> DebugMesh;

	virtual void GenerateMesh(UProceduralMeshComponent* procMesh) override;
};
