// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "Constants.h"
#include <ProceduralMeshComponent.h>

/**
 * 
 */
class SILLYCRAFT_API ChunkMesher
{
public:
	ChunkMesher();
	~ChunkMesher();

	static void MeshChunk(AChunk* chunk);
};
