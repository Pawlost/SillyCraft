// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunks/ChunkRMCActor.h"

// Sets default values
AChunkRMCActor::AChunkRMCActor()
{	// Here we register our new component
	RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RealtimeMeshComponent"));
	RealtimeMeshComponent->SetMobility(EComponentMobility::Stationary);
	RealtimeMeshComponent->SetGenerateOverlapEvents(false);
	RealtimeMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	SetRootComponent(RealtimeMeshComponent);
}

void AChunkRMCActor::InitParameters(TSubclassOf<UChunkMesherBase> chunkMesherBase) const
{
	
}

void AChunkRMCActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}
