// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/Area/AreaChunkSpawnerBase.h"

#include "Meshers/MeshingStructs/ChunkFaceParams.h"

void AAreaChunkSpawnerBase::ModifyVoxelAtChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition,
                                               const FVoxel& VoxelId)
{
	if (EditHandle.IsValid() && !EditHandle.IsReady())
	{
		return;
	}

	if (ChunkGrid.Contains(chunkGridPosition))
	{
		auto foundChunk = ChunkGrid.Find(chunkGridPosition);

		if (foundChunk == nullptr || !foundChunk->IsValid())
		{
			return;
		}

		auto chunk = *foundChunk;

		VoxelGenerator->ChangeVoxelIdInChunk(chunk, voxelPosition, FVoxel(VoxelId));


		EditHandle = Async(EAsyncExecution::LargeThreadPool, [this, chunk]()
		{
			FChunkFaceParams chunkParams;
			chunk->IsActive = false;
			GenerateChunkMesh(chunkParams, chunk->GridPosition);
			FChunkFaceParams sideParams;

			for (int32 s = 0; s < FACE_SIDE_COUNT; s++)
			{
				auto sideChunk = chunkParams.ChunkParams.SideChunks[s];
				if (sideChunk.IsValid())
				{
					sideChunk->IsActive = false;
					GenerateChunkMesh(sideParams, sideChunk->GridPosition);
				}
			}
		}).Share();
	}
}

void AAreaChunkSpawnerBase::BeginPlay()
{
	Super::BeginPlay();
	if (WorldCenter){
		CenterGridPosition = WorldPositionToChunkGridPosition(GetTransform().GetLocation());
	}

	if (SpawnCenterChunk)
	{
		//Spawn center chunk
		SpawnChunk(CenterGridPosition);
		FChunkFaceParams faceParams;
		faceParams.ChunkParams.ExecutedOnMainThread = true;
		GenerateChunkMesh(faceParams, CenterGridPosition);
	}

	SpawnChunks();
}

//Running on main thread may cause deadlock
void AAreaChunkSpawnerBase::GenerateChunkMesh(FChunkFaceParams& chunkParams, const FIntVector& chunkGridPosition)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Area Mesh generation prepartion")
#endif

	if (!IsValid(this) || !ChunkGrid.Contains(chunkGridPosition))
	{
		return;
	}

	TSharedPtr<FChunkStruct>& chunk = *ChunkGrid.Find(chunkGridPosition);

	if (chunk->IsActive)
	{
		return;
	}
	
	chunkParams.ChunkParams.SpawnerPtr = this;
	chunkParams.ChunkParams.ShowBorders = BufferZone == 0;
	chunkParams.ChunkParams.OriginalChunk = chunk;

	if (!WorldCenter)
	{
		chunkParams.ChunkParams.ActorAttachmentRules = FAttachmentTransformRules::KeepRelativeTransform;
	}
	
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::TopDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::BottomDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::RightDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::LeftDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::FrontDirection);
	AddChunkFromGrid(chunkParams, FGridDirectionToFace::BackDirection);

	if (chunk->ChunkMeshActor == nullptr)
	{
		UnusedActors.Dequeue(chunk->ChunkMeshActor);
	}
	
	//Mesh could be spawned on a Async Thread similary to voxel models but it is not done so to showcase real time speed of mesh generation (requirement for bachelor thesis)
	VoxelGenerator->GenerateMesh(chunkParams);

	if (!chunk->HasMesh)
	{
		UnusedActors.Enqueue(chunk->ChunkMeshActor);
		chunk->ChunkMeshActor = nullptr;
	}

	for (auto sideChunk : chunkParams.ChunkParams.SideChunks)
	{
		if (sideChunk == nullptr || !sideChunk.IsValid())
		{
			return;
		}
	}

	chunk->IsActive = true;
}

void AAreaChunkSpawnerBase::SpawnChunk(const FIntVector& chunkGridPosition, TSharedFuture<void>* asyncExecution)
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

	InitChunk(Chunk, chunkGridPosition, asyncExecution);

	Mutex.Lock();
	ChunkGrid.Add(chunkGridPosition, Chunk);
	Mutex.Unlock();
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
	SpawnHandle = Async(EAsyncExecution::Thread, [this]()
	{
		GenerateArea();
	}).Share();
}

void AAreaChunkSpawnerBase::DespawnChunks()
{
	AsyncTask(ENamedThreads::BackgroundThreadPriority, [this]()
	{
		TArray<FIntVector> chunkKeys;
		ChunkGrid.GetKeys(chunkKeys);

		for (auto chunkKey : chunkKeys)
		{
			if (FVector::Distance(FVector(CenterGridPosition), FVector(chunkKey)) > SpawnZone + DespawnZone)
			{
				if (!IsValid(this) || !ChunkGrid.Contains(chunkKey))
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
