// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "Constants.h"
#include "BlockRegistry.h"
#include <array>
#include <ProceduralMeshComponent.h>

/**
 * 
 */
class SILLYCRAFT_API ChunkMesher
{
private:
	enum Faces {
		Front = 0, Back = 1, Right = 2, Left = 3, Top = 4, Bottom = 5
	};

	struct IntVector {
		int X;
		int Y;
		int Z;

		FVector GetVertice() const;
	};

	struct Face {
		IntVector v1;
		IntVector v2;
		IntVector v3;
		IntVector v4;

		bool visible;
	};

	struct Voxel {
		Face face[6];

		int ID;
	};

	BlockRegistry* m_registry;
	void AddIndice(const int& index, TArray<int>& indice) const;
	int MakeIndex(const int& x, const int& y, const int& z) const;
	void NaiveGreedyMeshing(std::array<Voxel, Constants::ChunkSize3D>& voxels, const AChunk* chunk) const;
	void GreedyMeshing(const std::array<Voxel, Constants::ChunkSize3D>& vectors) const;

public:
	ChunkMesher(BlockRegistry* generator);
	~ChunkMesher();

	void MeshChunk(const AChunk* chunk) const;
};
