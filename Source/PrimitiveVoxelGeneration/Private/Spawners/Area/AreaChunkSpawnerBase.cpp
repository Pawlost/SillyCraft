// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/Area/AreaChunkSpawnerBase.h"

void AAreaChunkSpawnerBase::ModifyVoxelAtChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition,
                                               const FVoxel& VoxelId)
{
	if (ChunkGrid.Contains(chunkGridPosition))
	{
		if (EditHandle.IsValid() && !EditHandle.IsReady() || (SpawnHandle.IsValid() && !SpawnHandle.IsReady()))
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
		chunk->IsActive = false;
		
		EditHandle = Async(EAsyncExecution::TaskGraph, [this, chunk]()
		{
			FChunkFaceParams chunkParams;
			GenerateChunkMesh(chunkParams, chunk->GridPosition);

			for (int32 s = 0; s < FACE_SIDE_COUNT; s++)
			{
				auto sideChunk = chunkParams.ChunkParams.SideChunks[s];
				if (sideChunk.IsValid())
				{
					FChunkFaceParams sideParams;
					GenerateChunkMesh(chunkParams, chunk->GridPosition);
				}
			}
		});
	}
}

void AAreaChunkSpawnerBase::BeginPlay()
{
	Super::BeginPlay();
	CenterGridPosition = WorldPositionToChunkGridPosition(GetTransform().GetLocation());
	SpawnChunks();
}

//Running on main thread may cause deadlock
void AAreaChunkSpawnerBase::GenerateChunkMesh(FChunkFaceParams& chunkParams, const FIntVector& chunkGridPosition)
{
	if (!IsValid(this) || !ChunkGrid.Contains(chunkGridPosition))
	{
		return;
	}
	
	TSharedPtr<FChunkStruct>& chunk = *ChunkGrid.Find(chunkGridPosition);

	if (chunk->IsActive)
	{
		return;
	}
	
	TWeakObjectPtr<AChunkRmcActor> ActorPtr;
	if (chunk->ChunkMeshActor == nullptr)
	{
		if (!UnusedActors.Dequeue(ActorPtr))
		{
			ActorPtr = nullptr;
		}
	}
	else
	{
		ActorPtr = chunk->ChunkMeshActor;
	}

	chunk->ChunkMeshActor = GetChunkActor(chunk->GridPosition, ActorPtr, chunkParams.ChunkParams.ExecutedOnMainThread).Get();
	chunkParams.ChunkParams.ShowBorders = ShowBorderChunks;
	chunkParams.ChunkParams.OriginalChunk = chunk;
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::TopDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::BottomDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::RightDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::LeftDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::FrontDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::BackDirection);
	VoxelGenerator->GenerateMesh(chunkParams);

	if (!chunk->HasMesh)
	{
		UnusedActors.Enqueue(chunk->ChunkMeshActor);
		chunk->ChunkMeshActor = nullptr;
	}

	chunk->IsActive = true;
}

void AAreaChunkSpawnerBase::SpawnChunk(const FIntVector& chunkGridPosition)
{
	if (!IsValid(this) || ChunkGrid.Contains(chunkGridPosition))
	{
		return;
	}
	
	TSharedPtr<FChunkStruct> Chunk;
	if (!DespawnedChunks.Dequeue(Chunk))
	{
		Chunk = MakeShared<FChunkStruct>().ToSharedPtr();
	}

	InitChunk(Chunk, chunkGridPosition);

	ChunkGrid.Add(chunkGridPosition, Chunk);
}

void AAreaChunkSpawnerBase::AddChunkFromGrid(FChunkFaceParams& params, const FGridDirectionToFace& faceDirection)
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

void AAreaChunkSpawnerBase::SpawnChunks()
{

	//Spawn center chunk to stand on
	SpawnChunk(CenterGridPosition);
	FChunkFaceParams faceParams;
	faceParams.ChunkParams.ExecutedOnMainThread = true;
	GenerateChunkMesh(faceParams, CenterGridPosition);
	
	SpawnHandle = Async(EAsyncExecution::ThreadPool, [this]()
	{
		GenerateArea();
	});
}

void AAreaChunkSpawnerBase::DespawnChunks()
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

			//TODO: rewrite despawn
			if (FVector::Distance(chunkPosition, gridCenterPosition) > DespawnRadiusFromGeneratedArea)
			{
				if (!IsValid(this))
				{
					return;
				}

				auto chunk = *ChunkGrid.Find(chunkKey);

				if (chunk->ChunkMeshActor != nullptr)
				{
					chunk->ChunkMeshActor->ClearMesh();
					UnusedActors.Enqueue(chunk->ChunkMeshActor);
					chunk->ChunkMeshActor = nullptr;
				}

				chunk->IsActive = false;
				chunk->ChunkVoxelTypeTable.Reset();
				
				Mutex.Lock();
				ChunkGrid.Remove(chunkKey);
				Mutex.Unlock();
				
				DespawnedChunks.Enqueue(chunk);
			}
		}
	});
}
