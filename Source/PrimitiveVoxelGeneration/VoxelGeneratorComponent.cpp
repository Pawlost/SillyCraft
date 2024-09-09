// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelGeneratorComponent.h"

#include "DiffUtils.h"
#include "RenderGraphEvent.h"
#include "OperatorOverloads.h"

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
	Super::BeginPlay();
}

bool UVoxelGeneratorComponent::IsPlayerInChunkBounds() const
{
	auto location = GetOwner()->GetActorTransform().GetLocation();
	return TVector<double>::PointsAreNear(location, CurrentChunkLocation, RenderDistanceBounds);
}

void UVoxelGeneratorComponent::DespawnChunks(const FIntVector ChunkMinDistance, const FIntVector ChunkMaxDistance)
{
	for (auto Element : SpawnedChunks)
	{
		if(Element.Key < ChunkMinDistance || Element.Key.X > ChunkMaxDistance.X)
		{
			ShowDebugVector(CurrentChunkLocation, FColor::Green);
			
			AsyncTask(ENamedThreads::GameThread, [this, Element]()
			{
				if(Element.Value)
				{
					if(GetWorld()->ContainsActor(Element.Value))
					{
						Element.Value->Destroy();
					}
				}
					
				SpawnedChunks.Remove(Element.Key);
			});
		}
	}
}

void UVoxelGeneratorComponent::SpawnChunks(const FIntVector ChunkMinDistance, const FIntVector ChunkMaxDistance)
{
	for (int x = ChunkMinDistance.X; x <= ChunkMaxDistance.X; x++)
	{
		for (int y = ChunkMinDistance.Y; y <= ChunkMaxDistance.Y; y++)
		{
			for (int z = ChunkMinDistance.Z; z <= ChunkMaxDistance.Z; z++)
			{
				auto vector = FIntVector(x, y, z);
				if(!SpawnedChunks.Contains(vector))
				{
					AsyncTask(ENamedThreads::GameThread, [this, vector]()
					{
						auto chunk = GetWorld()->SpawnActor<AChunk>();
						auto spawnedChunkLocation = FVector(vector.X, vector.Y, vector.Z);
						spawnedChunkLocation *= ChunkSize;
						chunk->SetActorLocation(spawnedChunkLocation);
						chunk->SetLockLocation(true);
						SpawnedChunks.Add(vector, chunk);
					});
				}
			}
		}
	}
}

void UVoxelGeneratorComponent::ShowDebugVector(TVector<double>& vector, FColor color)
{
	GEngine->AddOnScreenDebugMessage(-1, 6.0f, color,
						 *(FString::Printf(
							 TEXT("New Chunk Location - X:%f | Y:%f | Z:%f"),
							 vector.X, vector.Y, vector.Z)));
}

// Called every frame
void UVoxelGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto playerLocation = GetOwner()->GetActorTransform().GetLocation();
	
	GEngine->AddOnScreenDebugMessage(-1, DebugTime, FColor::Green,
			 *(FString::Printf(
				 TEXT("Stamina - X:%f | Y:%f | Z:%f"), playerLocation.X, playerLocation.Y, playerLocation.Z)));
	
	if (!IsPlayerInChunkBounds())
	{
		{
			const auto GridVector = playerLocation / RenderDistanceBounds;
	
			CurrentChunkLocation.X = FMath::Floor(GridVector.X); 
			CurrentChunkLocation.Y = FMath::Floor(GridVector.Y);
			CurrentChunkLocation.Z = FMath::Floor(GridVector.Z);
		}

		auto ChunkMinDistance =  FIntVector(CurrentChunkLocation.X , CurrentChunkLocation.Y, CurrentChunkLocation.Z);
		auto ChunkMaxDistance = ChunkMinDistance;

		{
			const auto GenerationDistanceV = FIntVector(GenerationDistance);
			ChunkMinDistance = ChunkMinDistance - GenerationDistanceV;
			ChunkMaxDistance = ChunkMaxDistance + GenerationDistanceV;
		}
	
		CurrentChunkLocation *= RenderDistanceBounds;
		
		AsyncTask(ENamedThreads::AnyThread, [this, ChunkMinDistance, ChunkMaxDistance]()
		{
			DespawnChunks(ChunkMinDistance, ChunkMaxDistance);
			SpawnChunks(ChunkMinDistance, ChunkMaxDistance);
		});
	}
}

/*
* 		TRACE_CPUPROFILER_EVENT_SCOPE(VoxelForLoop)
		auto test = GetWorld()->SpawnActor(TestCube);
		test->SetActorLocation(location);
 */