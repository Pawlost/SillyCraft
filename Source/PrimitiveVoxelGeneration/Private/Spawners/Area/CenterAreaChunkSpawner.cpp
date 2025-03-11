// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/Area/CenterAreaChunkSpawner.h"

UE_DISABLE_OPTIMIZATION

void ACenterAreaChunkSpawner::GenerateArea()
{
	TArray<FIntVector> SpawnPositionsArray;
	SpawnPositionsArray.Add(CenterGridPosition);

	SpawnChunk(CenterGridPosition);
	
	FChunkFaceParams faceParams;
	GenerateChunkMesh(faceParams, CenterGridPosition);

	while (IsValid(this) && !SpawnPositionsArray.IsEmpty())
	{
		auto centerPosition = SpawnPositionsArray[0];
		//Front
		SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::FrontDirection,
					   SpawnZone);

		//Top
		SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::TopDirection,
					   ChunksAboveSpawner);

		//Right
		SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::RightDirection,
					   SpawnZone);

		//Left
		SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::LeftDirection,
					   SpawnZone);

		//Back
		SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::BackDirection,
					   SpawnZone);

		//Bottom
		SpawnSideChunk(centerPosition, SpawnPositionsArray, 
					   FGridDirectionToFace::BottomDirection, ChunksBelowSpawner);

		//Mesh
		if (FVector::Distance(FVector(CenterGridPosition), FVector(centerPosition)) < SpawnZone)
		{
			GenerateChunkMesh(faceParams, centerPosition);
		}
	}

	SpawnPositionsArray.RemoveAt(0, EAllowShrinking::No);
}

void ACenterAreaChunkSpawner::SpawnSideChunk(const FIntVector& centerPosition, TArray<FIntVector>& SpawnPositions,
                                             FGridDirectionToFace direction, const double& maxSpawnDistance)
{
	auto position = centerPosition + direction.Direction;

	auto chunkPtr = ChunkGrid.Find(position);

	if (chunkPtr != nullptr)
	{
		auto chunk = *chunkPtr;
		if (chunk->IsActive)
		{
			return;
		}
	}

	if (FVector::Distance(FVector(CenterGridPosition), FVector(position)) < maxSpawnDistance + BufferZone)
	{
		if (chunkPtr == nullptr)
		{
			SpawnChunk(position);
		}

		SpawnPositions.AddUnique(position);
	}
}