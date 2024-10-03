// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelGeneratorComponent.h"

#include "Chunks/ChunkBase.h"
#include "Chunks/ChunkGridData.h"
#include "Chunks/ChunkSettings.h"
#include "OperatorOverloads.h"

// Sets default values for this component's properties
UVoxelGeneratorComponent::UVoxelGeneratorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	ChunkGridData = CreateDefaultSubobject<UChunkGridData>("Chunk grid");
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UVoxelGeneratorComponent::BeginPlay()
{
	checkf(VoxelTypeTable, TEXT("Voxel generator mush have a voxel table"));
	checkf(ChunkTemplate, TEXT("Chunk template is not set"));
	
	ChunkSize = VoxelSize * ChunkSideSizeInVoxels;
	RenderDistanceBounds = ChunkSize * GenerationDistance;

	SpawnedChunks = MakeShared<TMap<FIntVector, TWeakObjectPtr<UChunkBase>>>();
		
	auto settings = MakeShared<FChunkSettings>();
	{
		settings->SetVoxelSize(VoxelSize);
		settings->SetChunkSizeInVoxels(ChunkSideSizeInVoxels);
		settings->SetMaximumElevation(MaximumElevation);
	}

	// First chunk Id is empty space
	auto rowNames = VoxelTypeTable->GetRowNames();
	
	ChunkGridData->SetSpawnedChunks(SpawnedChunks);
	ChunkGridData->SetChunkSettings(settings);
	ChunkGridData->SetVoxelTypes(VoxelTypeTable);
	
	UpdateCurrentChunkLocation();
	
	if(MoveActorToSurface)
	{
		UChunkBase* Chunk;
		CreateChunk(Chunk, FIntVector(0));
		
		auto location = GetOwner()->GetTransform().GetLocation().GridSnap(VoxelSize);
		location.Z = Chunk->GetHighestElevationAtPosition(location.X, location.Y) + DistanceFromSurfaceInVoxels * VoxelSize;
		GetOwner()->SetActorLocation(location);

		Chunk->Despawn();
	}

	UpdateCurrentChunkLocation();
	SpawnChunks(CurrentChunkLocation.ChunkMinCoords, CurrentChunkLocation.ChunkMaxCoords);
	
	Super::BeginPlay();
}

void UVoxelGeneratorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


bool UVoxelGeneratorComponent::IsPlayerInChunkBounds() const
{
	auto location = GetOwner()->GetActorTransform().GetLocation();

	auto distance = VoxelSize * ChunkSideSizeInVoxels;
	
	GEngine->AddOnScreenDebugMessage(-1, DebugTime, FColor::Green,
									 *(FString::Printf(
										 TEXT("Generator Position - X:%f | Y:%f | Z:%f"),
										 location.X, location.Y,
										 location.Z)));

	GEngine->AddOnScreenDebugMessage(-1, DebugTime, FColor::Green,
							 *(FString::Printf(
								 TEXT("Current chunk location - X:%f | Y:%f | Z:%f"),
								 CurrentChunkLocation.Coords.X, CurrentChunkLocation.Coords.Y,
								 CurrentChunkLocation.Coords.Z)));
	
	return TVector<double>::PointsAreNear(location, CurrentChunkLocation.Coords, distance);
}

void UVoxelGeneratorComponent::DespawnChunks(const FIntVector& ChunkMinDistance, const FIntVector& ChunkMaxDistance)
{
	// Async check of every chunk outside bounds
	//AsyncTask(ENamedThreads::AnyThread, [this, ChunkMinDistance, ChunkMaxDistance]()
	{
		auto identity = FIntVector(1, 1, 1);
		auto minDistancePlus = ChunkMinDistance - identity;
		auto maxDistancePlus = ChunkMaxDistance + identity;

		TArray<FIntVector> ChunkCoords;	
		SpawnedChunks->GetKeys(ChunkCoords);
		for (auto Coord : ChunkCoords)
		{
			if (Coord < minDistancePlus || Coord > maxDistancePlus)
			{
				SpawnedChunks.Get()->FindChecked(Coord)->Despawn();
			}else if (Coord < ChunkMinDistance || Coord > ChunkMaxDistance)
			{
				SpawnedChunks.Get()->FindChecked(Coord)->RemoveMeshAsync();
			}
		}
	}//); 
}

void UVoxelGeneratorComponent::SpawnChunks(const FIntVector ChunkMinDistance, const FIntVector ChunkMaxDistance)
{
	AsyncTask(ENamedThreads::AnyThread, [this, ChunkMinDistance, ChunkMaxDistance]()
	{
		DespawnChunks(ChunkMinDistance, ChunkMaxDistance);

		for (int x = ChunkMinDistance.X - 1; x <= ChunkMaxDistance.X; x++)
		{
			for (int y = ChunkMinDistance.Y - 1; y <= ChunkMaxDistance.Y; y++)
			{
				for (int z = ChunkMinDistance.Z - 1; z <= ChunkMaxDistance.Z; z++)
				{
					auto gridCoords = FIntVector(x, y, z);
					
					if (!ChunkGridData->IsChunkInGrid(gridCoords))
					{
						UChunkBase* Chunk;

						auto handle = Async(EAsyncExecution::TaskGraphMainThread , [this, &Chunk, gridCoords]() mutable
						{
							if(!IsValid(this)){
								return;
							}
						
							CreateChunk(Chunk, gridCoords);

							Chunk->Spawn();
						});

						handle.Wait();
						
						if(IsValid(this) && Chunk != nullptr && IsValid(Chunk))
						{
							Chunk->GenerateVoxels();
						}
					}
				}
			}
		}


		for (int x = ChunkMinDistance.X; x < ChunkMaxDistance.X; x++)
		{
			for (int y = ChunkMinDistance.Y; y < ChunkMaxDistance.Y; y++)
			{
				for (int z = ChunkMinDistance.Z; z < ChunkMaxDistance.Z; z++)
				{
					auto Chunk = ChunkGridData->GetChunkPtr(FIntVector(x, y, z));

					if(Chunk == nullptr)
					{
						continue;
					}

					if(!Chunk->HasMesh())
					{
						auto handle = Async(EAsyncExecution::TaskGraphMainThread , [&Chunk]()
						{
							Chunk->RemoveMesh();
						});

						handle.Wait();
					}

					if(Chunk == nullptr || !Chunk.IsValid())
					{
						continue;
					}
					
					Chunk->GenerateMesh();
				}
			}
		}
	});
}

void UVoxelGeneratorComponent::UpdateCurrentChunkLocation()
{
	{
		const auto generatorGridLocation = GetOwner()->GetActorTransform().GetLocation() / ChunkSize;

		CurrentChunkLocation.Coords.X = FMath::Floor(generatorGridLocation.X);
		CurrentChunkLocation.Coords.Y = FMath::Floor(generatorGridLocation.Y);
		CurrentChunkLocation.Coords.Z = FMath::Floor(generatorGridLocation.Z);
	}

	auto chunkLoc = static_cast<FIntVector>(CurrentChunkLocation.Coords);
	
	CurrentChunkLocation.Coords *= ChunkSize;
	{
		const auto GenerationDistanceV = FIntVector(GenerationDistance);
		CurrentChunkLocation.ChunkMinCoords = chunkLoc - GenerationDistanceV;
		CurrentChunkLocation.ChunkMaxCoords = chunkLoc + GenerationDistanceV;
	}
}

void UVoxelGeneratorComponent::CreateChunk(UChunkBase*& Chunk, FIntVector ChunkCoordinates)
{
	Chunk = NewObject<UChunkBase>(this, ChunkTemplate);
	auto chunkGridData = MakeWeakObjectPtr<UChunkGridData>(ChunkGridData);
	Chunk->AddToGrid(chunkGridData, ChunkCoordinates);

	if(ShowChunkBorders)
	{
		Chunk->ShowChunkBorders();
	}
}

// Called every frame
void UVoxelGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsPlayerInChunkBounds())
	{
		UpdateCurrentChunkLocation();
		
		auto ChunkMinDistance = CurrentChunkLocation.ChunkMinCoords;
		auto ChunkMaxDistance = CurrentChunkLocation.ChunkMaxCoords;

		SpawnChunks(ChunkMinDistance, ChunkMaxDistance);
	}
}

void UVoxelGeneratorComponent::ShowDebugVector(TVector<double>& vector, FColor color)
{
	GEngine->AddOnScreenDebugMessage(-1, 6.0f, color,
									 *(FString::Printf(
										 TEXT("New Chunk Location - X:%f | Y:%f | Z:%f"),
										 vector.X, vector.Y, vector.Z)));
}