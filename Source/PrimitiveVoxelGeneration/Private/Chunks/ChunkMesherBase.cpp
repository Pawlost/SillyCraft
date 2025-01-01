// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkMesherBase.h"

#include "Chunks/ChunkGridData.h"
#include "Chunks/ChunkSettings.h"

void UChunkMesherBase::AddToGrid(const TWeakObjectPtr<UChunkGridData> chunkGridData, FIntVector& chunkGridPos)
{
	ChunkGridData = chunkGridData;
	ChunkGridPos = chunkGridPos;

	auto chunkPtr =  MakeWeakObjectPtr<UChunkMesherBase>(this);
	chunkGridData->AddChunkToGrid(chunkPtr, chunkGridPos);
}

void UChunkMesherBase::Spawn(bool lockLocation)
{
	auto spawnedChunkLocation = FVector(ChunkGridPos.X, ChunkGridPos.Y, ChunkGridPos.Z) * ChunkGridData->GetChunkSettings()->GetChunkSize();

	auto parameters = FActorSpawnParameters();
	parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	
	ChunkActor = GetWorld()->SpawnActor<ARealtimeMeshActor>(parameters);
	ChunkActor->SetActorLocation(spawnedChunkLocation);
	ChunkActor->SetLockLocation(lockLocation);
}

void UChunkMesherBase::RemoveMeshAsync() const
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		RemoveMesh();
	});
}

void UChunkMesherBase::RemoveMesh() const
{

	if(!IsValid(this))
	{
		return;
	}
	
	FScopeLock Lock(&DataGuard);
	// TODO: Reimplement remove mesh
	/*if(ChunkActor != nullptr && IsValid(ChunkActor)){
		auto procMesh = ChunkActor->GetProceduralMeshComponent();
		if(procMesh.IsValid(false,true))
		{
			procMesh->ClearAllMeshSections();
		}
	}*/
}

void UChunkMesherBase::Despawn() const
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if(!IsValid(this))
		{
			return;
		}
		
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

bool UChunkMesherBase::IsEmpty() const
{
	return Empty;
}

// position of exact chunk
double UChunkMesherBase::GetHighestElevationAtPosition(double posX, double posY)
{
	return ChunkGridData->GetChunkSettings()->GetMaximumElevation();
}