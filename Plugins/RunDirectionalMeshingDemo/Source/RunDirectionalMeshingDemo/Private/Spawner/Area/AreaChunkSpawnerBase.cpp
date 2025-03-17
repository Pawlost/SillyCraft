#include "Spawner/Area/AreaChunkSpawnerBase.h"
#include "Mesher/MeshingStructs/ChunkFaceParams.h"


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

		VoxelGenerator->ChangeVoxelIdInChunk(chunk, voxelPosition, VoxelId);

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
	checkf(VoxelGenerator, TEXT("Voxel generator must set"));
	
	if (WorldCenter)
	{
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
	chunkParams.ChunkParams.OriginalChunk = chunk;
	chunkParams.ChunkParams.ShowBorders = ShowChunkBorders;
	chunkParams.ChunkParams.WorldTransform = WorldCenter;

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
	Async(EAsyncExecution::Thread, [this]()
	{
		GenerateArea();
	});
}
