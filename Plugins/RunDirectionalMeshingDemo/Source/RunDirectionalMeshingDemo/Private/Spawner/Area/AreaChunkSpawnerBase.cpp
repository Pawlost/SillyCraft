#include "Spawner/Area/AreaChunkSpawnerBase.h"
#include "Mesher/MeshingUtils/MesherVariables.h"
#include "Voxel/Generators/VoxelGeneratorBase.h"

void AAreaChunkSpawnerBase::ChangeVoxelInChunk(const FIntVector& ChunkGridPosition, const FIntVector& VoxelPosition,
                                               const FName& VoxelId)
{
	if (EditHandle.IsValid() && !EditHandle.IsReady())
	{
		return;
	}

	if (ChunkGrid.Contains(ChunkGridPosition))
	{
		const auto FoundChunk = ChunkGrid.Find(ChunkGridPosition);

		if (FoundChunk == nullptr || !FoundChunk->IsValid())
		{
			return;
		}

		auto Chunk = *FoundChunk;

		VoxelGenerator->ChangeUnknownVoxelIdInChunk(Chunk, VoxelPosition, VoxelId);

		EditHandle = Async(EAsyncExecution::ThreadPool, [this, Chunk]()
		{
			FMesherVariables MesherVars;
			Chunk->bIsActive = false;
			GenerateChunkMesh(MesherVars, Chunk->GridPosition);
			FMesherVariables SideMesherVars;

			for (int32 s = 0; s < CHUNK_FACE_COUNT; s++)
			{
				auto SideChunk = MesherVars.ChunkParams.SideChunks[s];
				if (SideChunk.IsValid())
				{
					SideChunk->bIsActive = false;
					GenerateChunkMesh(SideMesherVars, SideChunk->GridPosition);
				}
			}
		}).Share();
	}
}

void AAreaChunkSpawnerBase::BeginPlay()
{
	Super::BeginPlay();
	checkf(VoxelGenerator, TEXT("Voxel generator must set"));

	if (UseWorldCenter)
	{
		CenterGridPosition = WorldPositionToChunkGridPosition(GetTransform().GetLocation());
	}

	if (SpawnCenterChunk)
	{
		//Spawn center chunk
		SpawnChunk(CenterGridPosition);
		FMesherVariables MesherVars;
		MesherVars.ChunkParams.ExecutedOnMainThread = true;
		GenerateChunkMesh(MesherVars, CenterGridPosition);
	}

	SpawnChunks();
}

//Running on main thread may cause deadlock
void AAreaChunkSpawnerBase::GenerateChunkMesh(FMesherVariables& MesherVars, const FIntVector& ChunkGridPosition)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Area Mesh generation prepartion")
#endif

	if (!IsValid(this) || !ChunkGrid.Contains(ChunkGridPosition))
	{
		return;
	}

	const TSharedPtr<FChunk>& Chunk = *ChunkGrid.Find(ChunkGridPosition);

	if (Chunk->bIsActive)
	{
		return;
	}

	MesherVars.ChunkParams.SpawnerPtr = this;
	MesherVars.ChunkParams.OriginalChunk = Chunk;
	MesherVars.ChunkParams.ShowBorders = ShowChunkBorders;
	MesherVars.ChunkParams.WorldTransform = UseWorldCenter;

	AddChunkFromGrid(MesherVars, FFaceToDirection::TopDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::BottomDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::RightDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::LeftDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::FrontDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::BackDirection);

	if (Chunk->ChunkMeshActor == nullptr)
	{
		UnusedActors.Dequeue(Chunk->ChunkMeshActor);
	}

	//Mesh could be spawned on a Async Thread similarly to voxel models but it is not done so to showcase real time speed of mesh generation (requirement for bachelor thesis)
	VoxelGenerator->GenerateMesh(MesherVars);

	if (!Chunk->bHasMesh)
	{
		UnusedActors.Enqueue(Chunk->ChunkMeshActor);
		Chunk->ChunkMeshActor = nullptr;
	}

	for (auto SideChunk : MesherVars.ChunkParams.SideChunks)
	{
		if (SideChunk == nullptr || !SideChunk.IsValid())
		{
			// If not all side chunks were available the chunk is not fully active and needs to be remeshed
			return;
		}
	}

	Chunk->bIsActive = true;
}

void AAreaChunkSpawnerBase::SpawnChunk(const FIntVector& ChunkGridPosition, TSharedFuture<void>* AsyncExecution)
{
	if (!IsValid(this) || ChunkGrid.Contains(ChunkGridPosition))
	{
		// Don't spawn chunk if there is one located at the grid position
		return;
	}

	TSharedPtr<FChunk> Chunk = MakeShared<FChunk>();
	AddChunkToGrid(Chunk, ChunkGridPosition, AsyncExecution);

	Mutex.Lock();
	ChunkGrid.Add(ChunkGridPosition, Chunk);
	Mutex.Unlock();
}

void AAreaChunkSpawnerBase::AddChunkFromGrid(FMesherVariables& MesherVars, const FFaceToDirection& FaceDirection)
{
	const auto Chunk = ChunkGrid.Find(MesherVars.ChunkParams.OriginalChunk->GridPosition + FaceDirection.Direction);
	if (Chunk == nullptr)
	{
		AddSideChunk(MesherVars, FaceDirection.FaceSide, nullptr);
	}
	else
	{
		AddSideChunk(MesherVars, FaceDirection.FaceSide, *Chunk);
	}
}

void AAreaChunkSpawnerBase::SpawnChunks()
{
	Async(EAsyncExecution::Thread, [this]()
	{
		GenerateArea();
	});
}
