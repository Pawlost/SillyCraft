// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/AreaChunkSpawner.h"

#include "MeshingStructs/DirectionToFace.h"

void AAreaChunkSpawner::BeginPlay()
{
	Super::BeginPlay();
	auto location = GetTransform().GetLocation() / ChunkMesher->GetChunkSize();
	auto actorGridLocation = FIntVector(FMath::FloorToInt32(location.X), FMath::FloorToInt32(location.Y),
	                                    FMath::FloorToInt32(location.Z));
	GenerateArea(actorGridLocation);
}

void AAreaChunkSpawner::AddChunkFromGrid(FChunkFaceParams& params, const FDirectionToFace& faceDirection)
{
	auto chunk = ChunkGrid.Find(params.ChunkParams.OriginalChunk->GridPosition + faceDirection.Direction);
	if (chunk == nullptr)
	{
		AddSideChunk(params, faceDirection.FaceSide, nullptr);
	}
	else
	{
		AddSideChunk(params, faceDirection.FaceSide, *chunk);
	}
}

void AAreaChunkSpawner::GenerateArea(const FIntVector& gridPosition)
{
	auto minPosition = gridPosition - FIntVector(SpawnRadius);
	auto maxPosition = gridPosition + FIntVector(SpawnRadius);

	auto minExtendedBorder = minPosition;
	auto maxExtendedBorder = maxPosition;
	
	if (!ShowBorderChunks)
	{
		auto singleVector = FIntVector(1);
		minExtendedBorder -= singleVector;
		maxExtendedBorder += singleVector;
	}

	AsyncTask(ENamedThreads::AnyThread, [this, minExtendedBorder, maxExtendedBorder, minPosition, maxPosition]()
	{
		for (int32 x = minExtendedBorder.X; x < maxExtendedBorder.X; x++)
		{
			for (int32 y = minExtendedBorder.Y; y < maxExtendedBorder.Y; y++)
			{
				for (int32 z = minExtendedBorder.Z; z < maxExtendedBorder.Z; z++)
				{
					if (!IsValid(this))
					{
						return;
					}

					auto currentGridPosition = FIntVector(x, y, z);
					if (!ChunkGrid.Contains(currentGridPosition))
					{
						auto Chunk = MakeShared<FChunkStruct>().ToSharedPtr();
						Chunk->GridPosition = currentGridPosition;
						ChunkMesher->GenerateVoxels(Chunk);
						SpawnChunk(Chunk);
						ChunkGrid.Add(currentGridPosition, Chunk);
					}
				}
			}
		}

		for (int32 x = minPosition.X; x < maxPosition.X; x++)
		{
			for (int32 y = minPosition.Y; y < maxPosition.Y; y++)
			{
				for (int32 z = minPosition.Z; z < maxPosition.Z; z++)
				{
					auto currentGridPosition = FIntVector(x, y, z);
					if (!IsValid(this))
					{
						return;
					}
					if (ChunkGrid.Contains(currentGridPosition))
					{
						FChunkFaceParams chunkParams;
						chunkParams.ChunkParams.OriginalChunk = *ChunkGrid.Find(currentGridPosition);
						AddChunkFromGrid(chunkParams, FDirectionToFace::TopDirection);
						AddChunkFromGrid(chunkParams, FDirectionToFace::BottomDirection);
						AddChunkFromGrid(chunkParams, FDirectionToFace::RightDirection);
						AddChunkFromGrid(chunkParams, FDirectionToFace::LeftDirection);
						AddChunkFromGrid(chunkParams, FDirectionToFace::FrontDirection);
						AddChunkFromGrid(chunkParams, FDirectionToFace::BackDirection);
						ChunkMesher->GenerateMesh(chunkParams);
					}
				}
			}
		}
	});
}
