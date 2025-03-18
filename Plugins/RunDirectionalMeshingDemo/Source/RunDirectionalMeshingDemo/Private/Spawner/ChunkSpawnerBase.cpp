#include "Spawner/ChunkSpawnerBase.h"

#include "Mesher/MeshingUtils/MesherVariables.h"
#include "Voxel/Generators/VoxelGeneratorBase.h"

void AChunkSpawnerBase::BeginPlay()
{
	// Check if the template is valid
	if (VoxelGeneratorBlueprint)
	{
		// Create the component
		VoxelGenerator = NewObject<UVoxelGeneratorBase>(this, VoxelGeneratorBlueprint);

		if (VoxelGenerator)
		{
			// Register the component (required for it to work properly)
			VoxelGenerator->RegisterComponent();
		}
	}

	checkf(VoxelGenerator, TEXT("Voxel generator must be valid"));

	Super::BeginPlay();
}

double AChunkSpawnerBase::GetHighestElevationAtLocation(const FVector& Location) const
{
	return VoxelGenerator->GetHighestElevationAtLocation(Location);
}

void AChunkSpawnerBase::ChangeVoxelAtHit(const FVector& HitPosition, const FVector& HitNormal, const FName& VoxelName,
                                      const bool bPick)
{
	FVector AdjustedNormal;
	if (bPick)
	{
		AdjustedNormal.X = FMath::Clamp(HitNormal.X, 0, 1);
		AdjustedNormal.Y = FMath::Clamp(HitNormal.Y, 0, 1);
		AdjustedNormal.Z = FMath::Clamp(HitNormal.Z, 0, 1);
	}
	else
	{
		AdjustedNormal.X = -FMath::Clamp(HitNormal.X, -1, 0);
		AdjustedNormal.Y = -FMath::Clamp(HitNormal.Y, -1, 0);
		AdjustedNormal.Z = -FMath::Clamp(HitNormal.Z, -1, 0);
	}

	// Adjust position based on normal
	auto Position = HitPosition - AdjustedNormal * VoxelGenerator->GetVoxelSize();
	
	if (!UseWorldCenter)
	{
		// Transform hit position to local coordinates, if they are enabled
		Position -= GetActorLocation();	
	}

	const auto ChunkGridPosition = WorldPositionToChunkGridPosition(Position);

	// Precise voxel position in chunk
	const auto VoxelPosition = FIntVector(
		(Position - FVector(ChunkGridPosition * VoxelGenerator->GetChunkAxisSize())) / VoxelGenerator
		->GetVoxelSize());

	ChangeVoxelInChunk(ChunkGridPosition, VoxelPosition, VoxelName);
}

void AChunkSpawnerBase::AddSideChunk(FMesherVariables& MeshVar, EFaceDirection Direction,
                                     const TSharedPtr<FChunk>& Chunk)
{
	const auto DirectionIndex = static_cast<uint8>(Direction);
	MeshVar.ChunkParams.SideChunks[DirectionIndex] = Chunk.IsValid() ? Chunk : nullptr;
}

void AChunkSpawnerBase::AddChunkToGrid(TSharedPtr<FChunk>& Chunk,
                                  const FIntVector& GridPosition, TSharedFuture<void>* AsyncExecution) const
{
	Chunk->GridPosition = GridPosition;

	if (!Chunk->bIsInitialized)
	{
		// Initialize and request memory for voxel grid
		Chunk->VoxelGrid.SetNum(VoxelGenerator->GetVoxelCountPerChunk());
	}

	Chunk->bIsInitialized = true;
	if (AsyncExecution != nullptr)
	{
		// Generate voxels on async thread if promise is expected
		*AsyncExecution = Async(EAsyncExecution::ThreadPool, [this, Chunk]()
		{
			VoxelGenerator->GenerateVoxels(*Chunk.Get());
		}).Share();

	}else
	{
		VoxelGenerator->GenerateVoxels(*Chunk);
	}
}

FIntVector AChunkSpawnerBase::WorldPositionToChunkGridPosition(const FVector& WorldPosition) const
{
	const auto Location = WorldPosition / VoxelGenerator->GetChunkAxisSize();
	return FIntVector(FMath::FloorToInt32(Location.X), FMath::FloorToInt32(Location.Y),
	                  FMath::FloorToInt32(Location.Z));
}

void AChunkSpawnerBase::WaitForAllTasks(TArray<TSharedFuture<void>>& Tasks)
{
	for (auto Task : Tasks)
	{
		if (Task.IsValid() && !Task.IsReady())
		{
			Task.Wait();
		}
	}

	Tasks.Empty();
}