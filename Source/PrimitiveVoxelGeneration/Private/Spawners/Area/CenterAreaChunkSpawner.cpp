// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/Area/CenterAreaChunkSpawner.h"

UE_DISABLE_OPTIMIZATION

void ACenterAreaChunkSpawner::GenerateArea()
{
	TArray<FIntVector> SpawnPositionsArray;
	SpawnPositionsArray.Add(CenterGridPosition);
	FChunkFaceParams faceParams;
	while (IsValid(this) && !SpawnPositionsArray.IsEmpty())
	{
		FIntVector centerPosition = SpawnPositionsArray[0];
		if (FVector::Distance(FVector(CenterGridPosition), FVector(centerPosition)) < SpawnRadius)
		{
			//Front
			SpawnSideChunk(centerPosition,SpawnPositionsArray, FGridDirectionToFace::FrontDirection);

			//Right
			SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::RightDirection);

			//Left
			SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::LeftDirection);

			//Back
			SpawnSideChunk(centerPosition, SpawnPositionsArray, FGridDirectionToFace::BackDirection);

			for (int32 z = centerPosition.Z - ChunksBelowSpawner; z < centerPosition.Z + ChunksAboveSpawner; z++)
			{
				SpawnChunk(FIntVector(centerPosition.X, centerPosition.Y, z));
			}
		}
		//Mesh
		GenerateChunkMesh(faceParams, centerPosition);
		
		SpawnPositionsArray.RemoveAt(0, EAllowShrinking::No);
	}
}

void ACenterAreaChunkSpawner::SpawnSideChunk(const FIntVector& centerPosition, TArray<FIntVector>& SpawnPositionsArray, FGridDirectionToFace direction)
{
	auto position = centerPosition + direction.Direction;

	auto chunk = ChunkGrid.Find(position);

	if (chunk != nullptr && chunk->Get()->IsActive)
	{
		return;
	}
	
	if (chunk == nullptr)
	{
		SpawnChunk(position);
	}
	
	SpawnPositionsArray.AddUnique(position);
}
