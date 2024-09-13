// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelGeneratorComponent.h"

#include "OperatorOverloads.h"
#include "Kismet/GameplayStatics.h"

void UVoxelGeneratorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto Element : SpawnedChunks)
	{
		RemoveChunk(Element);
	}
	
	Super::EndPlay(EndPlayReason);
}

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
	UpdateCurrentChunkLocation();
	SpawnChunks(CurrentChunkLocation.ChunkMinCoords, CurrentChunkLocation.ChunkMaxCoords);

	ChunkSettingsPtr = MakeShared<FUChunkSettings>();

	{
		auto settings = ChunkSettingsPtr.Get();
		settings->SetSeed(Seed);
		settings->SetVoxelSize(VoxelSize);
		settings->SetChunkWidthInBlocks(ChunkWidthInBlocks);
		settings->SetVoxelTypes(VoxelTypeTable);
	}
	
	Super::BeginPlay();
}

bool UVoxelGeneratorComponent::IsPlayerInChunkBounds() const
{
	auto location = GetOwner()->GetActorTransform().GetLocation();
	return TVector<double>::PointsAreNear(location, CurrentChunkLocation.Coords, RenderDistanceBounds);
}

void UVoxelGeneratorComponent::RemoveChunk(const TTuple<TIntVector3<int>, AChunkActor*>& Element)
{
	// Remove chunk actor on GameThread in order to prevent mutual thread access
	AsyncTask(ENamedThreads::GameThread, [this, Element]()
	{
		// Check if actor has not been garbage collected by UE
		if (IsValid(Element.Value) && GetWorld()->ContainsActor(Element.Value))
		{
			// If not than despawn it
			Element.Value->Destroy();
		}
					
		// Remove despawned element from map
		SpawnedChunks.Remove(Element.Key);
	});
}

void UVoxelGeneratorComponent::DespawnChunks(const FIntVector ChunkMinDistance, const FIntVector ChunkMaxDistance)
{
	// Async check of every chunk outside bounds
	AsyncTask(ENamedThreads::AnyThread, [this, ChunkMinDistance, ChunkMaxDistance]()
	{
		for (auto Element : SpawnedChunks)
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
		for (int x = ChunkMinDistance.X; x <= ChunkMaxDistance.X; x++)
		{
			for (int y = ChunkMinDistance.Y; y <= ChunkMaxDistance.Y; y++)
			{
				for (int z = ChunkMinDistance.Z; z <= ChunkMaxDistance.Z; z++)
				{
					auto vector = FIntVector(x, y, z);
					if (!SpawnedChunks.Contains(vector))
					{
						AsyncTask(ENamedThreads::GameThread, [this, vector]()
						{
							auto transform = FTransform::Identity;

							{
								auto spawnedChunkLocation = FVector(vector.X, vector.Y, vector.Z) * ChunkSize;
								transform.SetLocation(spawnedChunkLocation);
							}

							auto chunk = GetWorld()->SpawnActorDeferred<AChunkActor>(AChunkActor::StaticClass(), transform,
								nullptr, nullptr,
								ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding);

							chunk->SetChunkSettings(ChunkTemplate, ChunkSettingsPtr);
							chunk->SetLockLocation(true);
							SpawnedChunks.Add(vector, chunk);
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