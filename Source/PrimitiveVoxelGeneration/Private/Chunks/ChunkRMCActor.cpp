// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunks/ChunkRMCActor.h"

// Sets default values
AChunkRMCActor::AChunkRMCActor()
{
	// Here we register our new component
	RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RealtimeMeshComponent"));
	RealtimeMeshComponent->SetMobility(EComponentMobility::Stationary);
	RealtimeMeshComponent->SetGenerateOverlapEvents(false);
	SetRootComponent(RealtimeMeshComponent);
}

void AChunkRMCActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}
