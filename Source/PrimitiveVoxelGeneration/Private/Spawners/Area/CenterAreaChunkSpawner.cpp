// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/Area/CenterAreaChunkSpawner.h"

void ACenterAreaChunkSpawner::GenerateArea()
{
	TArray<FIntVector> SpawnPositionsArray;
	SpawnPositionsArray.Add(CenterGridPosition);
	FChunkFaceParams faceParams;
	while (IsValid(this) && !SpawnPositionsArray.IsEmpty())
	{
		FIntVector centerPosition = SpawnPositionsArray[0];

		//Front
		SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::FrontDirection, SpawnRadius);

		//Top
		SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::TopDirection, ChunksAboveSpawner);

		//Right
		SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::RightDirection, SpawnRadius);

		//Left
		SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::LeftDirection, SpawnRadius);

		//Back
		SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::BackDirection, SpawnRadius);

		//Bottom
		SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::BottomDirection, ChunksBelowSpawner);

		//Mesh
		GenerateChunkMesh(faceParams, centerPosition);

		SpawnPositionsArray.RemoveAt(0, EAllowShrinking::No);
	}
}

void ACenterAreaChunkSpawner::SpawnSideChunk(const FIntVector& centerPosition, TArray<FIntVector>& SpawnPositionsArray,
                                             FGridDirectionToFace direction, const double& maxSpawnDistance)
{
	auto position = centerPosition + direction.Direction;

	auto chunk = ChunkGrid.Find(position);

	if (chunk != nullptr && chunk->Get()->IsActive)
	{
		return;
	}

	if (FVector::Distance(FVector(CenterGridPosition), FVector(position)) < maxSpawnDistance)
	{
		if (chunk == nullptr)
		{
			SpawnChunk(position);
		}

		SpawnPositionsArray.AddUnique(position);
	}
}
