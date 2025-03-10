// Fill out your copyright notice in the Description page of Project Settings.pp[p

#include "Spawners/Area/CubicAreaChunkSpawner.h"

void ACubicAreaChunkSpawner::GenerateArea()
{
	auto minPosition = CenterGridPosition - FIntVector(SpawnRadius, SpawnRadius, ChunksBelowSpawner);
	auto maxPosition = CenterGridPosition + FIntVector(SpawnRadius, SpawnRadius, ChunksAboveSpawner);

	for (int32 x = minPosition.X; x < maxPosition.X; x++)
	{
		for (int32 y = minPosition.Y; y < maxPosition.Y; y++)
		{
			for (int32 z = minPosition.Z; z < maxPosition.Z; z++)
			{
				auto currentGridPosition = FIntVector(x, y, z);
				SpawnChunk(currentGridPosition);
			}
		}
	}

	FChunkFaceParams chunkParams;
	for (int32 x = minPosition.X; x < maxPosition.X; x++)
	{
		for (int32 y = minPosition.Y; y < maxPosition.Y; y++)
		{
			for (int32 z = minPosition.Z; z < maxPosition.Z; z++)
			{
				auto currentGridPosition = FIntVector(x, y, z);
				GenerateChunkMesh(chunkParams, currentGridPosition);
			}
		}
	}
}
