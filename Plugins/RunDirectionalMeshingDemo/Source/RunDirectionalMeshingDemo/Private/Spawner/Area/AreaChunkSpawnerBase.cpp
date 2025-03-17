﻿#include "Spawner/Area/AreaChunkSpawnerBase.h"
#include "Mesher/MeshingUtils/MesherVariables.h"


void AAreaChunkSpawnerBase::ChangeVoxelInChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition,
                                               const FName& VoxelId)
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

		VoxelGenerator->ChangeUnknownVoxelIdInChunk(chunk, voxelPosition, VoxelId);

		EditHandle = Async(EAsyncExecution::LargeThreadPool, [this, chunk]()
		{
			FMesherVariables chunkParams;
			chunk->bIsActive = false;
			GenerateChunkMesh(chunkParams, chunk->GridPosition);
			FMesherVariables sideParams;

			for (int32 s = 0; s < CHUNK_FACE_COUNT; s++)
			{
				auto sideChunk = chunkParams.ChunkParams.SideChunks[s];
				if (sideChunk.IsValid())
				{
					sideChunk->bIsActive = false;
					GenerateChunkMesh(sideParams, sideChunk->GridPosition);
				}
			}
		}).Share();
	}
}

void AAreaChunkSpawnerBase::BeginPlay()
{
	Super::BeginPlay();
	checkf(VoxelGenerator, TEXT("Voxel generator must set"));
	
	if (LocalChunkTransform)
	{
		CenterGridPosition = WorldPositionToChunkGridPosition(GetTransform().GetLocation());
	}

	if (SpawnCenterChunk)
	{
		//Spawn center chunk
		SpawnChunk(CenterGridPosition);
		FMesherVariables faceParams;
		faceParams.ChunkParams.ExecutedOnMainThread = true;
		GenerateChunkMesh(faceParams, CenterGridPosition);
	}

	SpawnChunks();
}

//Running on main thread may cause deadlock
void AAreaChunkSpawnerBase::GenerateChunkMesh(FMesherVariables& chunkParams, const FIntVector& chunkGridPosition)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Area Mesh generation prepartion")
#endif

	if (!IsValid(this) || !ChunkGrid.Contains(chunkGridPosition))
	{
		return;
	}

	TSharedPtr<FChunk>& chunk = *ChunkGrid.Find(chunkGridPosition);

	if (chunk->bIsActive)
	{
		return;
	}

	chunkParams.ChunkParams.SpawnerPtr = this;
	chunkParams.ChunkParams.OriginalChunk = chunk;
	chunkParams.ChunkParams.ShowBorders = ShowChunkBorders;
	chunkParams.ChunkParams.LocalTransform = LocalChunkTransform;

	AddChunkFromGrid(chunkParams, FFaceToDirection::TopDirection);
	AddChunkFromGrid(chunkParams, FFaceToDirection::BottomDirection);
	AddChunkFromGrid(chunkParams, FFaceToDirection::RightDirection);
	AddChunkFromGrid(chunkParams, FFaceToDirection::LeftDirection);
	AddChunkFromGrid(chunkParams, FFaceToDirection::FrontDirection);
	AddChunkFromGrid(chunkParams, FFaceToDirection::BackDirection);

	if (chunk->ChunkMeshActor == nullptr)
	{
		UnusedActors.Dequeue(chunk->ChunkMeshActor);
	}

	//Mesh could be spawned on a Async Thread similary to voxel models but it is not done so to showcase real time speed of mesh generation (requirement for bachelor thesis)
	VoxelGenerator->GenerateMesh(chunkParams);

	if (!chunk->bHasMesh)
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

	chunk->bIsActive = true;
}

void AAreaChunkSpawnerBase::SpawnChunk(const FIntVector& chunkGridPosition, TSharedFuture<void>* asyncExecution)
{
	if (!IsValid(this) || ChunkGrid.Contains(chunkGridPosition))
	{
		return;
	}

	TSharedPtr<FChunk> Chunk;
	if (!DespawnedChunks.Dequeue(Chunk))
	{
		Chunk = MakeShared<FChunk>().ToSharedPtr();
	}

	AddChunkToGrid(Chunk, chunkGridPosition, asyncExecution);

	Mutex.Lock();
	ChunkGrid.Add(chunkGridPosition, Chunk);
	Mutex.Unlock();
}

void AAreaChunkSpawnerBase::AddChunkFromGrid(FMesherVariables& params, const FFaceToDirection& faceDirection)
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
	Async(EAsyncExecution::Thread, [this]()
	{
		GenerateArea();
	});
}
