﻿// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/SingleChunkSpawner.h"

void ASingleChunkSpawner::BeginPlay()
{
	Super::BeginPlay();
	SingleChunk = MakeShared<FChunkStruct>();
	SingleChunk->GridPosition = SingleChunkGridPosition;
	SpawnChunk(SingleChunk);
	FChunkFaceParams params;
	params.ChunkParams.OriginalChunk = SingleChunk;
	AddSideChunk(params, EFaceDirection::Top, nullptr);
	AddSideChunk(params, EFaceDirection::Bottom, nullptr);
	AddSideChunk(params, EFaceDirection::Front, nullptr);
	AddSideChunk(params, EFaceDirection::Back, nullptr);
	AddSideChunk(params, EFaceDirection::Right, nullptr);
	AddSideChunk(params, EFaceDirection::Left, nullptr);
	ChunkMesher->GenerateMesh(params);
}
