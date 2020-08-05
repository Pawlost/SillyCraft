// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "Constants.h"
#include "BlockRegistry.h"
#include <ProceduralMeshComponent.h>

/**
 * 
 */
class SILLYCRAFT_API ChunkMesher
{
private:
	BlockRegistry* m_registry;
public:
	ChunkMesher(BlockRegistry* generator);
	~ChunkMesher();

	void MeshChunk(AChunk* chunk);
};
