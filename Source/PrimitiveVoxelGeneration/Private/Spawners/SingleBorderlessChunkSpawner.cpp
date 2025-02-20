// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/SingleBorderlessChunkSpawner.h"

#include "Spawners/SingleChunkSpawner.h"

void ASingleBorderlessChunkSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnChunk(SingleChunk, SingleChunkGridPosition);
	ChunkParams.ChunkParams.OriginalChunk = &SingleChunk;

	SpawnSideChunk(EFaceDirection::Top, FIntVector(0, 0, 1));
	SpawnSideChunk(EFaceDirection::Bottom, FIntVector(0, 0, -1));
	SpawnSideChunk(EFaceDirection::Front, FIntVector(-1, 0, 0));
	SpawnSideChunk(EFaceDirection::Back, FIntVector(1, 0, 0));
	SpawnSideChunk(EFaceDirection::Right, FIntVector(0, -1, 0));
	SpawnSideChunk(EFaceDirection::Left, FIntVector(0, 1, 0));

	ChunkMesher->GenerateMesh(ChunkParams);
}

void ASingleBorderlessChunkSpawner::SpawnSideChunk(EFaceDirection faceDirection, FIntVector side)
{
	auto index = static_cast<uint8>(faceDirection);
	SpawnChunk(SideChunk[index], SingleChunkGridPosition + side);
	AddSideChunk(ChunkParams, faceDirection, &SideChunk[index]);
}
