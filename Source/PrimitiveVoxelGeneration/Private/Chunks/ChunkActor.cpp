// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkActor.h"
#include "Chunks/ChunkBase.h"

TWeakObjectPtr<UProceduralMeshComponent> AChunkActor::GetProceduralMeshComponent() const
{
	return ProceduralMeshComponent;
}

AChunkActor::AChunkActor()
{
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("MeshComponent");
	SetRootComponent(ProceduralMeshComponent);
}

// Called when the game starts or when spawned
void AChunkActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AChunkActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

