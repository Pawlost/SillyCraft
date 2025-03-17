﻿// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawner/Single/SingleBorderlessChunkSpawner.h"

#include "Mesher/MeshingStructs/MesherVariables.h"

void ASingleBorderlessChunkSpawner::StartMeshing()
{
	FMesherVariables ChunkParams;
	ChunkParams.ChunkParams.SpawnerPtr = this;
	ChunkParams.ChunkParams.OriginalChunk = SingleChunk;
	ChunkParams.ChunkParams.WorldTransform = WorldCenter;

	SpawnSideChunk(ChunkParams, FGridDirectionToFace::TopDirection);
	SpawnSideChunk(ChunkParams, FGridDirectionToFace::BottomDirection);
	SpawnSideChunk(ChunkParams, FGridDirectionToFace::BackDirection);
	SpawnSideChunk(ChunkParams, FGridDirectionToFace::FrontDirection);
	SpawnSideChunk(ChunkParams, FGridDirectionToFace::RightDirection);
	SpawnSideChunk(ChunkParams, FGridDirectionToFace::LeftDirection);

	VoxelGenerator->GenerateMesh(ChunkParams);
}

void ASingleBorderlessChunkSpawner::SpawnSideChunk(FMesherVariables& chunkParams, const FGridDirectionToFace& faceDirection )
{
	auto index = static_cast<uint8>(faceDirection.FaceSide);
	auto chunk = MakeShared<FChunk>().ToSharedPtr();
	SideChunk[index] = chunk;
	auto gridPosition = SingleChunkGridPosition + faceDirection.Direction;
	InitChunk(chunk, gridPosition);
	AddSideChunk(chunkParams, faceDirection.FaceSide, SideChunk[index]);
}
