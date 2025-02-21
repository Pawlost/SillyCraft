// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/AreaChunkSpawner.h"

#include "MeshingStructs/DirectionToFace.h"

UE_DISABLE_OPTIMIZATION

void AAreaChunkSpawner::BeginPlay()
{
	Super::BeginPlay();

	auto location =  GetTransform().GetLocation() / ChunkMesher->GetChunkSize();
	auto actorGridLocation = FIntVector(FMath::FloorToInt32(location.X) , FMath::FloorToInt32(location.Y) , FMath::FloorToInt32(location.Z));
	GenerateArea(actorGridLocation);
}

void AAreaChunkSpawner::AddChunkFromGrid(FChunkFaceParams& params, const FDirectionToFace& faceDirection)
{
	auto chunk = ChunkGrid.Find(params.ChunkParams.OriginalChunk->GridPosition + faceDirection.Direction);
	if (chunk == nullptr)
	{
		AddSideChunk(params, faceDirection.FaceSide, nullptr);
	}else
	{
		AddSideChunk(params, faceDirection.FaceSide, *chunk);
	}
}

void AAreaChunkSpawner::BeginDestroy()
{
	Handle.Wait();
	Super::BeginDestroy();
}

void AAreaChunkSpawner::GenerateArea(const FIntVector& gridPosition)
{

	auto minPosition = gridPosition - FIntVector(SpawnRadius);
	auto maxPosition = gridPosition + FIntVector(SpawnRadius);
	AsyncTask(ENamedThreads::AnyThread, [this, minPosition, maxPosition]()
	{
		for (int32 x = minPosition.X - 1; x < maxPosition.X + 1; x++)
		{
			for (int32 y = minPosition.Y - 1; y < maxPosition.Y + 1; y++)
			{
				for (int32 z = minPosition.Z - 1; z < maxPosition.Z + 1; z++)
				{
					if (!IsValid(this))
					{
						return;
					}
					
					auto currentGridPosition = FIntVector(x, y, z);
					if(!ChunkGrid.Contains(currentGridPosition))
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

		for (auto chunkLocation : ChunkGrid)
		{
			if (!IsValid(this))
			{
				return;
			}
			
			if (!chunkLocation.Value->HasMesh)
			{
				FChunkFaceParams chunkParams;
				chunkParams.ChunkParams.OriginalChunk = chunkLocation.Value.ToSharedRef();
				AddChunkFromGrid(chunkParams, FDirectionToFace::TopDirection);
				AddChunkFromGrid(chunkParams, FDirectionToFace::BottomDirection);
				AddChunkFromGrid(chunkParams, FDirectionToFace::RightDirection);
				AddChunkFromGrid(chunkParams, FDirectionToFace::LeftDirection);
				AddChunkFromGrid(chunkParams, FDirectionToFace::FrontDirection);
				AddChunkFromGrid(chunkParams, FDirectionToFace::BackDirection);
				ChunkMesher->GenerateMesh(chunkParams);
			}
		}
	 });
}
