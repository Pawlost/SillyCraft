// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/SingleBorderlessChunkSpawner.h"

#include "MeshingStructs/DirectionToFace.h"
#include "Spawners/SingleChunkSpawner.h"

void ASingleBorderlessChunkSpawner::BeginPlay()
{
	Super::BeginPlay();
	SingleChunk = MakeShared<FChunkStruct>();
	
	SpawnChunk(SingleChunk, SingleChunkGridPosition);
	ChunkParams.ChunkParams.OriginalChunk = SingleChunk;

	SpawnSideChunk(FDirectionToFace::TopDirection);
	SpawnSideChunk(FDirectionToFace::BottomDirection);
	SpawnSideChunk(FDirectionToFace::BackDirection);
	SpawnSideChunk(FDirectionToFace::FrontDirection);
	SpawnSideChunk(FDirectionToFace::RightDirection);
	SpawnSideChunk(FDirectionToFace::LeftDirection);

	ChunkMesher->GenerateMesh(ChunkParams);
}

void ASingleBorderlessChunkSpawner::SpawnSideChunk(const FDirectionToFace& faceDirection)
{
	auto index = static_cast<uint8>(faceDirection.FaceSide);
	auto chunk = MakeShared<FChunkStruct>().ToSharedPtr();
	SideChunk[index] = chunk;
	SpawnChunk(chunk, SingleChunkGridPosition + faceDirection.Direction);
	AddSideChunk(ChunkParams, faceDirection.FaceSide, SideChunk[index]);
}
