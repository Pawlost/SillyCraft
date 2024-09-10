// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"

AChunk::AChunk()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("MeshComponent");
}

void AChunk::SetMesherClass(const TSubclassOf<UChunkMesherBase>& MesherClass)
{
	ChunkMesherClass = MesherClass;
}

TSubclassOf<UChunkMesherBase> AChunk::GetMesherClass()
{
	return ChunkMesherClass;
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	checkf(ChunkMesherClass, TEXT("You must provide a valid chunk mesher class."));

	ChunkMesher = NewObject<UChunkMesherBase>(this, ChunkMesherClass);

	AsyncTask(ENamedThreads::AnyThread, [this]()
	{
		ChunkMesher->GenerateMesh(MeshComponent);	
	});

	Super::BeginPlay();
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

