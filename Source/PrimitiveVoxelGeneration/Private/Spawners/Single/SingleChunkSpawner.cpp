// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/Single/SingleChunkSpawner.h"

#include "Meshers/MeshingStructs/ChunkFaceParams.h"

void ASingleChunkSpawner::StartMeshing()
{
	FChunkFaceParams params;
	params.ChunkParams.SpawnerPtr = this;
	params.ChunkParams.OriginalChunk = SingleChunk;
	AddSideChunk(params, EFaceDirection::Top, nullptr);
	AddSideChunk(params, EFaceDirection::Bottom, nullptr);
	AddSideChunk(params, EFaceDirection::Front, nullptr);
	AddSideChunk(params, EFaceDirection::Back, nullptr);
	AddSideChunk(params, EFaceDirection::Right, nullptr);
	AddSideChunk(params, EFaceDirection::Left, nullptr);
	params.ChunkParams.ShowBorders = true;
	VoxelGenerator->GenerateMesh(params);
}
