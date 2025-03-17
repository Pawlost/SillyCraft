// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawner/Single/SingleChunkSpawner.h"

#include "Mesher/MeshingUtils/MesherVariables.h"

void ASingleChunkSpawner::BeginPlay()
{
	LocalChunkTransform = true;
	Super::BeginPlay();
}

void ASingleChunkSpawner::StartMeshing()
{
	FMesherVariables params;
	params.ChunkParams.SpawnerPtr = this;
	params.ChunkParams.OriginalChunk = SingleChunk;
	AddSideChunk(params, EFaceDirection::Top, nullptr);
	AddSideChunk(params, EFaceDirection::Bottom, nullptr);
	AddSideChunk(params, EFaceDirection::Front, nullptr);
	AddSideChunk(params, EFaceDirection::Back, nullptr);
	AddSideChunk(params, EFaceDirection::Right, nullptr);
	AddSideChunk(params, EFaceDirection::Left, nullptr);
	params.ChunkParams.ShowBorders = true;
	params.ChunkParams.WorldTransform = LocalChunkTransform;
	VoxelGenerator->GenerateMesh(params);
}
