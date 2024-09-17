// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkBase.h"

#include "Chunks/ChunkGridData.h"
#include "Chunks/ChunkSettings.h"
#include "Kismet/GameplayStatics.h"

void UChunkBase::AddToGrid(const TSharedPtr<FChunkGridData> chunkGridData, FIntVector& chunkGridPos)
{
	ChunkGridData = chunkGridData;
	ChunkGridPos = chunkGridPos;

	auto spawnedChunkLocation = FVector(chunkGridPos.X, chunkGridPos.Y, chunkGridPos.Z) * chunkGridData->GetChunkSettings()->GetChunkSize();
	SpawnTransform.SetLocation(spawnedChunkLocation);

	chunkGridData->AddChunkToGrid(this, chunkGridPos);
}

void UChunkBase::StartSpawn(bool lockLocation)
{
	AsyncTask(ENamedThreads::GameThread, [this, lockLocation]()
	{
		ChunkActor = GetWorld()->SpawnActorDeferred<AChunkActor>(AChunkActor::StaticClass(), SpawnTransform,
			nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding);

		ChunkActor->SetLockLocation(lockLocation);
	});
}

void UChunkBase::RemoveMesh() const
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		auto procMesh = ChunkActor->GetProceduralMeshComponent();
		if(procMesh.IsValid(false,true))
		{
			procMesh->ClearAllMeshSections();
		}
	});
}

void UChunkBase::FinishSpawn()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if(IsValid(ChunkActor))
		{
			UGameplayStatics::FinishSpawningActor(ChunkActor, SpawnTransform);
			Spawned = true;
		}
	});
}

void UChunkBase::Despawn() const
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
	// Check if actor has not been garbage collected by UE
	if (ChunkActor != nullptr && IsValid(ChunkActor) && GetWorld()->ContainsActor(ChunkActor))
	{
		// If not than despawn it
		ChunkActor->Destroy();
	}

		// Remove despawned element from map
		ChunkGridData->RemoveChunkFromGrid(ChunkGridPos);
	});
}

bool UChunkBase::IsSpawned() const
{
	return Spawned;
}
