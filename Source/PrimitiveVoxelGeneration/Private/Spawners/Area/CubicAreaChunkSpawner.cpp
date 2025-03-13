// Fill out your copyright notice in the Description page of Project Settings.pp[p

#include "Spawners/Area/CubicAreaChunkSpawner.h"

#include "Meshers/MeshingStructs/ChunkFaceParams.h"

void ACubicAreaChunkSpawner::GenerateArea()
{
	auto minPosition = CenterGridPosition - FIntVector(SpawnZone, SpawnZone, ChunksBelowSpawner);
	auto maxPosition = CenterGridPosition + FIntVector(SpawnZone, SpawnZone, ChunksAboveSpawner);

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
