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
		auto foundChunk = ChunkGrid.Find(ChunkGridPosition);

		if (foundChunk == nullptr || !foundChunk->IsValid())
		{
			return;
		}

		auto chunk = *foundChunk;

		VoxelGenerator->ChangeUnknownVoxelIdInChunk(chunk, VoxelPosition, VoxelId);

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
void AAreaChunkSpawnerBase::GenerateChunkMesh(FMesherVariables& MesherVars, const FIntVector& ChunkGridPosition)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Area Mesh generation prepartion")
#endif

	if (!IsValid(this) || !ChunkGrid.Contains(ChunkGridPosition))
	{
		return;
	}

	TSharedPtr<FChunk>& chunk = *ChunkGrid.Find(ChunkGridPosition);

	if (chunk->bIsActive)
	{
		return;
	}

	MesherVars.ChunkParams.SpawnerPtr = this;
	MesherVars.ChunkParams.OriginalChunk = chunk;
	MesherVars.ChunkParams.ShowBorders = ShowChunkBorders;
	MesherVars.ChunkParams.LocalTransform = LocalChunkTransform;

	AddChunkFromGrid(MesherVars, FFaceToDirection::TopDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::BottomDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::RightDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::LeftDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::FrontDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::BackDirection);

	if (chunk->ChunkMeshActor == nullptr)
	{
		UnusedActors.Dequeue(chunk->ChunkMeshActor);
	}

	//Mesh could be spawned on a Async Thread similarly to voxel models but it is not done so to showcase real time speed of mesh generation (requirement for bachelor thesis)
	VoxelGenerator->GenerateMesh(MesherVars);

	if (!chunk->bHasMesh)
	{
		UnusedActors.Enqueue(chunk->ChunkMeshActor);
		chunk->ChunkMeshActor = nullptr;
	}

	for (auto sideChunk : MesherVars.ChunkParams.SideChunks)
	{
		if (sideChunk == nullptr || !sideChunk.IsValid())
		{
			return;
		}
	}

	chunk->bIsActive = true;
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
