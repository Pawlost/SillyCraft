// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunks/ChunkRMCActor.h"
#include "RealtimeMeshSimple.h"

// Sets default values
AChunkRmcActor::AChunkRmcActor() 
{
	// Here we register our new component
	RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RealtimeMeshComponent"));
	RealtimeMeshComponent->SetMobility(EComponentMobility::Stationary);
	RealtimeMeshComponent->SetGenerateOverlapEvents(false);
	SetRootComponent(RealtimeMeshComponent);
}

void AChunkRmcActor::StartColliderGeneration(const FRealtimeMeshSectionKey& SectionKey, uint16 materialId) const
{
	auto RealtimeMesh = RealtimeMeshComponent->GetRealtimeMeshAs<
	URealtimeMeshSimple>();
	
	RealtimeMesh->UpdateSectionConfig(SectionKey, FRealtimeMeshSectionConfig(
									  ERealtimeMeshSectionDrawType::Static, materialId),
								  true).Share().Wait();
}

void AChunkRmcActor::OnConstruction(const FTransform& Transform)
{
	RealtimeMeshComponent->InitializeRealtimeMesh<URealtimeMeshSimple>();
	Super::OnConstruction(Transform);
}

void AChunkRmcActor::BeginDestroy()
{
	MeshColliderHandle.Wait();
	Super::BeginDestroy();
}
