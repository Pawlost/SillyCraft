// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkActor.h"

#include "Chunks/ChunkBase.h"

AChunkActor::AChunkActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("MeshComponent");
	SetRootComponent(MeshComponent);
}

void AChunkActor::SetChunkGridData(const TSharedPtr<FChunkGridData>& chunkGridData, const FIntVector& chunkGridPos)
{
	ChunkGridPos = chunkGridPos;
	ChunkGridData = chunkGridData;
	
	// TODO: add checkf
	auto chunkClass = ChunkGridData->GetChunkClass();
	Chunk = NewObject<UChunkBase>(this, chunkClass);
	
	Chunk->SetChunkGridData(ChunkGridData);

//	AsyncTask(ENamedThreads::AnyThread, [this]()
//	{
		Chunk->GenerateVoxels(ChunkGridPos);
//	});
}

int AChunkActor::VoxelIdAt(const int index) const
{
	return Chunk->VoxelAt(index);
}

// Called when the game starts or when spawned
void AChunkActor::BeginPlay()
{

	Chunk->GenerateMesh(MeshComponent, ChunkGridPos);	
	Super::BeginPlay();
}

// Called every frame
void AChunkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

