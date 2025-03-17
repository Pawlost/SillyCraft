#include "Spawner/ChunkSpawnerBase.h"

#include "Mesher/MeshingStructs/MesherVariables.h"

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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ChunkTemplate is nullptr!"));
	}

	Super::BeginPlay();
}

double AChunkSpawnerBase::GetHighestElevationAtLocation(const FVector& Location) const
{
	return VoxelGenerator->GetHighestElevationAtLocation(Location);
}

void AChunkSpawnerBase::ChangeVoxelAtHit(const FVector& HitPosition, const FVector& HitNormal, const FName& VoxelName,
                                      bool bPick)
{
	FVector adjustedNormal;
	if (bPick)
	{
		adjustedNormal.X = FMath::Clamp(HitNormal.X, 0, 1);
		adjustedNormal.Y = FMath::Clamp(HitNormal.Y, 0, 1);
		adjustedNormal.Z = FMath::Clamp(HitNormal.Z, 0, 1);
	}
	else
	{
		adjustedNormal.X = -FMath::Clamp(HitNormal.X, -1, 0);
		adjustedNormal.Y = -FMath::Clamp(HitNormal.Y, -1, 0);
		adjustedNormal.Z = -FMath::Clamp(HitNormal.Z, -1, 0);
	}
	
	auto position = HitPosition - adjustedNormal * VoxelGenerator->GetVoxelSize();
	
	if (!WorldCenter)
	{
		position -= GetActorLocation();	
	}
	
	auto chunkGridPosition = WorldPositionToChunkGridPosition(position);
	
	auto voxelPosition = FIntVector(
		(position - FVector(chunkGridPosition * VoxelGenerator->GetChunkAxisSize())) / VoxelGenerator
		->GetVoxelSize());

	ChangeVoxelInChunk(chunkGridPosition, voxelPosition, VoxelName);
}

void AChunkSpawnerBase::AddSideChunk(FMesherVariables& ChunkParams, EFaceDirection Direction,
                                     const TSharedPtr<FChunk>& Chunk)
{
	auto directionIndex = static_cast<uint8>(Direction);
	ChunkParams.ChunkParams.SideChunks[directionIndex] = Chunk.IsValid() ? Chunk : nullptr;
}

void AChunkSpawnerBase::AddChunkToGrid(TSharedPtr<FChunk>& Chunk,
                                  const FIntVector& GridPosition, TSharedFuture<void>* AsyncExecution) const
{
	//Actor must always be respawned because of stationary mesh

	Chunk->GridPosition = GridPosition;

	if (!Chunk->bIsInitialized)
	{
		Chunk->VoxelGrid.SetNum(VoxelGenerator->GetVoxelCountPerChunk());
	}

	Chunk->bIsInitialized = true;
	if (AsyncExecution != nullptr)
	{
		*AsyncExecution = Async(EAsyncExecution::LargeThreadPool, [this, Chunk]()
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
	auto location = WorldPosition / VoxelGenerator->GetChunkAxisSize();
	return FIntVector(FMath::FloorToInt32(location.X), FMath::FloorToInt32(location.Y),
	                  FMath::FloorToInt32(location.Z));
}
