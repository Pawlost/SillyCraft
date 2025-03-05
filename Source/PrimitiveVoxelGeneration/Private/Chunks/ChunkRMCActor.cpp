// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunks/ChunkRMCActor.h"
#include "RealtimeMeshSimple.h"

// Sets default values
AChunkRmcActor::AChunkRmcActor()
{
	// Here we register our new component
	RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RealtimeMeshComponent"));
	RealtimeMeshComponent->SetMobility(EComponentMobility::Movable);
	RealtimeMeshComponent->SetGenerateOverlapEvents(false);
	SetRootComponent(RealtimeMeshComponent);
}

void AChunkRmcActor::ClearMesh() const
{
	auto RealTimeMesh =
		RealtimeMeshComponent->GetRealtimeMeshAs<
			URealtimeMeshSimple>();

	FRealtimeMeshCollisionConfiguration config;
	config.bUseAsyncCook = false;
	RealTimeMesh->SetCollisionConfig(config);

	RealtimeMeshComponent->GetRealtimeMeshAs<
		URealtimeMeshSimple>()->RemoveSectionGroup(GroupKey);
}

void AChunkRmcActor::PrepareMesh() const
{
	auto RealTimeMesh =
		RealtimeMeshComponent->GetRealtimeMeshAs<
			URealtimeMeshSimple>();
	RealTimeMesh->SetCollisionConfig(DefaultConfig);
}

void AChunkRmcActor::BeginPlay()
{
	RealtimeMeshComponent->InitializeRealtimeMesh<URealtimeMeshSimple>();

	DefaultConfig = RealtimeMeshComponent->GetRealtimeMeshAs<
		URealtimeMeshSimple>()->GetCollisionConfig();
	
	Super::BeginPlay();
}

void AChunkRmcActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearMesh();
	Super::EndPlay(EndPlayReason);
}
