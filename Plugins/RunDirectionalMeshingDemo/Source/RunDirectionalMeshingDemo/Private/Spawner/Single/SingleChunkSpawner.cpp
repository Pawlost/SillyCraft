// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawner/Single/SingleChunkSpawner.h"

#include "Mesher/MeshingStructs/ChunkFaceParams.h"

void ASingleChunkSpawner::BeginPlay()
{
	WorldCenter = true;
	Super::BeginPlay();
}

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
	params.ChunkParams.WorldTransform = WorldCenter;
	VoxelGenerator->GenerateMesh(params);
}
