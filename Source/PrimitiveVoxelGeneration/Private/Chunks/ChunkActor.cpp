// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkActor.h"

#include "Chunks/ChunkBase.h"

AChunkActor::AChunkActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("MeshComponent");
	SetRootComponent(MeshComponent);
}

void AChunkActor::SetChunkSettings(const TSharedPtr<FChunkGridData>& chunkGridData, const FIntVector& chunkGridPos)
{
	ChunkGridPos = chunkGridPos;
	ChunkGridData = chunkGridData;
}

// Called when the game starts or when spawned
void AChunkActor::BeginPlay()
{
	// TODO: add checkf
	auto chunkClass = ChunkGridData->GetChunkClass();
	Chunk = NewObject<UChunkBase>(this, chunkClass);
	
	Chunk->SetChunkSettings(ChunkGridData->GetChunkSettings());

	AsyncTask(ENamedThreads::AnyThread, [this]()
	{
		Chunk->GenerateVoxels(ChunkGridPos);
		Chunk->GenerateMesh(MeshComponent, ChunkGridPos);	
	});

	Super::BeginPlay();
}

// Called every frame
void AChunkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

