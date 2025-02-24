// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/SingleBorderlessChunkSpawner.h"

#include "Meshers/MeshingStructs/GridDirectionToFace.h"
#include "Spawners/SingleChunkSpawner.h"

void ASingleBorderlessChunkSpawner::BeginPlay()
{
	Super::BeginPlay();
	SingleChunk = MakeShared<FChunkStruct>();

	SingleChunk->GridPosition = SingleChunkGridPosition;
	SpawnChunk(SingleChunk);
	ChunkParams.ChunkParams.OriginalChunk = SingleChunk;

	SpawnSideChunk(FGridDirectionToFace::TopDirection);
	SpawnSideChunk(FGridDirectionToFace::BottomDirection);
	SpawnSideChunk(FGridDirectionToFace::BackDirection);
	SpawnSideChunk(FGridDirectionToFace::FrontDirection);
	SpawnSideChunk(FGridDirectionToFace::RightDirection);
	SpawnSideChunk(FGridDirectionToFace::LeftDirection);

	ChunkMesher->GenerateMesh(ChunkParams);
}

void ASingleBorderlessChunkSpawner::SpawnSideChunk(const FGridDirectionToFace& faceDirection)
{
	auto index = static_cast<uint8>(faceDirection.FaceSide);
	auto chunk = MakeShared<FChunkStruct>().ToSharedPtr();
	SideChunk[index] = chunk;
	chunk->GridPosition = SingleChunkGridPosition + faceDirection.Direction;
	SpawnChunk(chunk);
	AddSideChunk(ChunkParams, faceDirection.FaceSide, SideChunk[index]);
}
