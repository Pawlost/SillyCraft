// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkActor.h"

#include "Chunks/ChunkBase.h"

AChunkActor::AChunkActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("MeshComponent");
	SetRootComponent(MeshComponent);
}

void AChunkActor::SetChunkSettings(const TSubclassOf<UChunkBase>& chunkClass, TSharedPtr<FGenerationSettings> chunkSettings)
{
	ChunkClass = chunkClass;
	ChunkSettings = chunkSettings;
}

// Called when the game starts or when spawned
void AChunkActor::BeginPlay()
{
	checkf(ChunkClass, TEXT("You must provide a valid chunk mesher class."));

	Chunk = NewObject<UChunkBase>(this, ChunkClass);
	Chunk->SetChunkSettings(ChunkSettings);

	//AsyncTask(ENamedThreads::AnyThread, [this]()
	//{
		auto location = GetActorLocation();
		Chunk->GenerateVoxels(location);
		Chunk->GenerateMesh(MeshComponent);	
//	});

	Super::BeginPlay();
}

// Called every frame
void AChunkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

