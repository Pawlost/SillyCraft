// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "Constants.h"
#include "BlockRegistry.h"
#include "FastCube.h"
#include <array>

/**
 * 
 */
class SILLYCRAFT_API ChunkMesher
{
private:
	struct IntVector {
		int X;
		int Y;
		int Z;

		bool operator==(const IntVector& a);
		bool operator!=(const IntVector& a);

		FVector GetVertice() const;
	};

	struct Face {
		IntVector v1;
		IntVector v2;
		IntVector v3;
		IntVector v4;

		bool placed;
		bool visible;
		Face* prevFace;
	};

	struct Voxel {
		Face face[6];

		int ID;
	};

	BlockRegistry* m_registry;
	void AddIndice(const int& index, TArray<int>& indice) const;
	void NaiveGreedyMeshing(std::array<Voxel, Constants::ChunkSize3D>& voxels, const AChunk& chunk, std::array<AChunk*, 6> chunkArray) const;
	void GreedyMeshing(std::array<Voxel, Constants::ChunkSize3D>& voxels, TMultiMap<int, Face*>& meshedFaces) const;
	static void Join(Face* face, Face* prevFace);
	static void JoinReversed(Face* face, Face* prevFace);
	static bool GreedyJoin(Face* face, Face* prevFace);

public:
	enum Faces {
		Front = 0, Back = 1, Right = 2, Left = 3, Top = 4, Bottom = 5
	};

	ChunkMesher(BlockRegistry* generator);
	~ChunkMesher();

	bool MeshChunk(AChunk& meshchunk, std::array<AChunk*, 6> chunkArray) const;

	void CreateFastCube(AFastCube& cube, FLinearColor color) const;
};