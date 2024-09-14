// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelGeneratorComponent.h"

#include "OperatorOverloads.h"
#include "Chunks/ChunkGridData.h"
#include "Chunks/ChunkSettings.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UVoxelGeneratorComponent::UVoxelGeneratorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UVoxelGeneratorComponent::BeginPlay()
{
	ChunkSize = VoxelSize * ChunkSideSizeInVoxels;
	RenderDistanceBounds = ChunkSize * GenerationDistance;

	SpawnedChunks = MakeShared<TMap<FIntVector, AChunkActor*>>();

	auto settings = MakeShared<FChunkSettings>();
	{
		settings->SetSeed(Seed);
		settings->SetVoxelSize(VoxelSize);
		settings->SetChunkSizeInVoxels(ChunkSideSizeInVoxels);
		settings->SetVoxelTypes(VoxelTypeTable);
		settings->SetMaximumElevation(MaximumElevation);
		settings->SetNoiseFrequency(NoiseFrequency);
	}
	
	ChunkGridPtr = MakeShared<FChunkGridData>();
	ChunkGridPtr->SetSpawnedChunks(SpawnedChunks);
	ChunkGridPtr->SetChunkClass(ChunkTemplate);
	ChunkGridPtr->SetChunkSettings(settings);
	
	UpdateCurrentChunkLocation();
	SpawnChunks(CurrentChunkLocation.ChunkMinCoords, CurrentChunkLocation.ChunkMaxCoords);
	
	Super::BeginPlay();
}

void UVoxelGeneratorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto Element : *SpawnedChunks.Get())
	{
		RemoveChunk(Element);
	}
	
	Super::EndPlay(EndPlayReason);
}


bool UVoxelGeneratorComponent::IsPlayerInChunkBounds() const
{
	auto location = GetOwner()->GetActorTransform().GetLocation();
	return TVector<double>::PointsAreNear(location, CurrentChunkLocation.Coords, VoxelSize * ChunkSideSizeInVoxels * GenerationDistance);
}

void UVoxelGeneratorComponent::RemoveChunk(const TTuple<TIntVector3<int>, AChunkActor*>& Chunk) const
{
	// Remove chunk actor on GameThread in order to prevent mutual thread access
	AsyncTask(ENamedThreads::GameThread, [this, Chunk]()
	{
		// Check if actor has not been garbage collected by UE
		if (IsValid(Chunk.Value) && GetWorld()->ContainsActor(Chunk.Value))
		{
			// If not than despawn it
			Chunk.Value->Destroy();
		}

		// Remove despawned element from map
		ChunkGridPtr->RemoveChunkFromGrid(Chunk.Key);
	});
}

void UVoxelGeneratorComponent::DespawnChunks(const FIntVector ChunkMinDistance, const FIntVector ChunkMaxDistance)
{
	// Async check of every chunk outside bounds
	AsyncTask(ENamedThreads::AnyThread, [this, ChunkMinDistance, ChunkMaxDistance]()
	{
		for (auto Element : *SpawnedChunks.Get())
		{
			if (Element.Key < ChunkMinDistance || Element.Key > ChunkMaxDistance)
			{
				RemoveChunk(Element);
			}
		}
	});
}

void UVoxelGeneratorComponent::SpawnChunks(const FIntVector ChunkMinDistance, const FIntVector ChunkMaxDistance)
{
	AsyncTask(ENamedThreads::AnyThread, [this, ChunkMinDistance, ChunkMaxDistance]()
	{
		for (int x = ChunkMinDistance.X; x < ChunkMaxDistance.X; x++)
		{
			for (int y = ChunkMinDistance.Y; y < ChunkMaxDistance.Y; y++)
			{
				for (int z = ChunkMinDistance.Z; z < ChunkMaxDistance.Z; z++)
				{
					auto gridCoords = FIntVector(x, y, z);
					
					if (!ChunkGridPtr->IsChunkInGrid(gridCoords))
					{
						auto transform = FTransform::Identity;

						{
							auto spawnedChunkLocation = FVector(gridCoords.X, gridCoords.Y, gridCoords.Z) * ChunkSize;
							transform.SetLocation(spawnedChunkLocation);
						}
						
						AsyncTask(ENamedThreads::GameThread, [this, gridCoords, transform]()
						{
							auto chunk = GetWorld()->SpawnActorDeferred<AChunkActor>(AChunkActor::StaticClass(), transform,
								nullptr, nullptr,
								ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding);

							chunk->SetChunkSettings(ChunkGridPtr, gridCoords);
							chunk->SetLockLocation(true);
							ChunkGridPtr->AddChunkToGrid(gridCoords, chunk);
							UGameplayStatics::FinishSpawningActor(chunk, transform);
						});
					}
				}
			}
		}
	});
}

void UVoxelGeneratorComponent::UpdateCurrentChunkLocation()
{
	auto playerLocation = GetOwner()->GetActorTransform().GetLocation();

	GEngine->AddOnScreenDebugMessage(-1, DebugTime, FColor::Green,
	                                 *(FString::Printf(
		                                 TEXT("Stamina - X:%f | Y:%f | Z:%f"), playerLocation.X, playerLocation.Y,
		                                 playerLocation.Z)));

	{
		const auto GridVector = playerLocation / RenderDistanceBounds;

		CurrentChunkLocation.Coords.X = FMath::Floor(GridVector.X);
		CurrentChunkLocation.Coords.Y = FMath::Floor(GridVector.Y);
		CurrentChunkLocation.Coords.Z = FMath::Floor(GridVector.Z);
	}

	auto chunkLoc = static_cast<FIntVector>(CurrentChunkLocation.Coords);
	CurrentChunkLocation.Coords *= RenderDistanceBounds;

	{
		const auto GenerationDistanceV = FIntVector(GenerationDistance);
		CurrentChunkLocation.ChunkMinCoords = chunkLoc - GenerationDistanceV;
		CurrentChunkLocation.ChunkMaxCoords = chunkLoc + GenerationDistanceV;
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

		DespawnChunks(ChunkMinDistance, ChunkMaxDistance);
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