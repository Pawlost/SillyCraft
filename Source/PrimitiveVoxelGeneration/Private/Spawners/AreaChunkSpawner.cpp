// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/AreaChunkSpawner.h"

UE_DISABLE_OPTIMIZATION

void AAreaChunkSpawner::ModifyVoxelAtChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition,
                                           const FVoxel& VoxelId)
{
	if (ChunkGrid.Contains(chunkGridPosition))
	{
		if (EditHandle.IsValid() && !EditHandle.IsReady())
		{
			return;
		}

		auto foundChunk = ChunkGrid.Find(chunkGridPosition);

		if (foundChunk == nullptr || !foundChunk->IsValid())
		{
			return;
		}

		auto chunk = *foundChunk;

		VoxelGenerator->ChangeVoxelIdInChunk(chunk, voxelPosition, FVoxel(VoxelId));

		EditHandle = Async(EAsyncExecution::TaskGraph, [this, chunk]()
		{
			FChunkFaceParams chunkParams;
			GenerateChunkMesh(chunkParams, chunk);

			for (int32 s = 0; s < FACE_SIDE_COUNT; s++)
			{
				auto sideChunk = chunkParams.ChunkParams.SideChunks[s];
				if (sideChunk.IsValid())
				{
					FChunkFaceParams sideParams;
					GenerateChunkMesh(sideParams, sideChunk);
				}
			}
		});
	}
}

void AAreaChunkSpawner::BeginPlay()
{
	Super::BeginPlay();
	ChunkGrid.Reserve(2 * SpawnRadius * SpawnRadius * SpawnRadius);
	CenterGridPosition = WorldPositionToChunkGridPosition(GetTransform().GetLocation());
	GenerateChunks();
}

void AAreaChunkSpawner::GenerateChunkMesh(FChunkFaceParams& chunkParams, const TSharedPtr<FChunkStruct>& chunk)
{
	chunkParams.ChunkParams.OriginalChunk = chunk;
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::TopDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::BottomDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::RightDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::LeftDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::FrontDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::BackDirection);
	VoxelGenerator->GenerateMesh(chunkParams);
}

void AAreaChunkSpawner::AddChunkFromGrid(FChunkFaceParams& params, const FGridDirectionToFace& faceDirection)
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

void AAreaChunkSpawner::GenerateChunks()
{
	SpawnHandle = Async(EAsyncExecution::ThreadPool, [this]()
	{
		auto minPosition = CenterGridPosition - FIntVector(SpawnRadius, SpawnRadius, ChunksBelowSpawner);
		auto maxPosition = CenterGridPosition + FIntVector(SpawnRadius, SpawnRadius, ChunksAboveSpawner);

		auto minExtendedBorder = minPosition;
		auto maxExtendedBorder = maxPosition;

		if (!ShowBorderChunks)
		{
			auto singleVector = FIntVector(1);
			minExtendedBorder -= singleVector;
			maxExtendedBorder += singleVector;
		}

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
						TSharedPtr<FChunkStruct> Chunk;
						if (!DespawnedChunks.Dequeue(Chunk))
						{
							Chunk = MakeShared<FChunkStruct>().ToSharedPtr();
						}

						InitChunk(Chunk, currentGridPosition);
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
						auto chunk = *ChunkGrid.Find(currentGridPosition);
						FChunkFaceParams chunkParams;
						GenerateChunkMesh(chunkParams, chunk);
					}
				}
			}
		}
	});
}

void AAreaChunkSpawner::DespawnChunks()
{
	AsyncTask(ENamedThreads::AnyThread, [this]()
	{
		SpawnHandle.Wait();
		auto gridCenterPosition = FVector(CenterGridPosition);
		TArray<FIntVector> chunkKeys;
		ChunkGrid.GetKeys(chunkKeys);

		for (auto chunkKey : chunkKeys)
		{
			auto chunkPosition = FVector(chunkKey);

			if (FVector::Distance(chunkPosition, gridCenterPosition) > DespawnRadius)
			{
				if (!IsValid(this))
				{
					return;
				}

				auto chunk = *ChunkGrid.Find(chunkKey);
				
				chunk->ChunkVoxelTypeTable.Reset();
				ChunkGrid.Remove(chunkKey);
				chunk->ChunkMeshActor->ClearMesh();

				DespawnedChunks.Enqueue(chunk);
			}
		}
	});
}
