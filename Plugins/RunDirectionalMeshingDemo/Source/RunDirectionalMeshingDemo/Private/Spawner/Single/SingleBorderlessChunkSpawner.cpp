// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawner/Single/SingleBorderlessChunkSpawner.h"

#include "Mesher/MeshingUtils/MesherVariables.h"

void ASingleBorderlessChunkSpawner::StartMeshing()
{
	FMesherVariables ChunkParams;
	ChunkParams.ChunkParams.SpawnerPtr = this;
	ChunkParams.ChunkParams.OriginalChunk = SingleChunk;
	ChunkParams.ChunkParams.LocalTransform = LocalChunkTransform;

	SpawnSideChunk(ChunkParams, FFaceToDirection::TopDirection);
	SpawnSideChunk(ChunkParams, FFaceToDirection::BottomDirection);
	SpawnSideChunk(ChunkParams, FFaceToDirection::BackDirection);
	SpawnSideChunk(ChunkParams, FFaceToDirection::FrontDirection);
	SpawnSideChunk(ChunkParams, FFaceToDirection::RightDirection);
	SpawnSideChunk(ChunkParams, FFaceToDirection::LeftDirection);

	VoxelGenerator->GenerateMesh(ChunkParams);
}

void ASingleBorderlessChunkSpawner::SpawnSideChunk(FMesherVariables& chunkParams, const FFaceToDirection& faceDirection )
{
	auto index = static_cast<uint8>(faceDirection.FaceSide);
	auto chunk = MakeShared<FChunk>().ToSharedPtr();
	SideChunk[index] = chunk;
	auto gridPosition = SingleChunkGridPosition + faceDirection.Direction;
	AddChunkToGrid(chunk, gridPosition);
	AddSideChunk(chunkParams, faceDirection.FaceSide, SideChunk[index]);
}
