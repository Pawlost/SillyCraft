// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/SingleChunkSpawner.h"

void ASingleChunkSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnChunk(SingleChunk, SingleChunkGridPosition);
	ChunkMesher->GenerateMesh(SingleChunk);
}