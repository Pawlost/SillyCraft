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

//TODO: rewrite

void AChunkRmcActor::ClearMesh() const
{

	auto RealTimeMesh =
		RealtimeMeshComponent->GetRealtimeMeshAs<
			URealtimeMeshSimple>();

	FRealtimeMeshCollisionConfiguration config;
	config.bUseAsyncCook = false;
	RealTimeMesh->SetCollisionConfig(config);
	
	for (auto sectionConfig : SectionConfigs)
	{
		auto key = FRealtimeMeshSectionKey::CreateForPolyGroup(GroupKey, sectionConfig.Value);
		RealTimeMesh->UpdateSectionConfig(key, FRealtimeMeshSectionConfig(
											  ERealtimeMeshSectionDrawType::Static, sectionConfig.Value),
										  false);
	}
	
	RealtimeMeshComponent->GetRealtimeMeshAs<
		URealtimeMeshSimple>()->RemoveSectionGroup(GroupKey);
}

void AChunkRmcActor::AddSectionConfig(uint16 materialId)
{
	auto RealTimeMesh =
		RealtimeMeshComponent->GetRealtimeMeshAs<
			URealtimeMeshSimple>();
	;
	RealTimeMesh->SetCollisionConfig(defaultConfig);
	
	auto key = FRealtimeMeshSectionKey::CreateForPolyGroup(GroupKey, materialId);
	RealTimeMesh->UpdateSectionConfig(key, FRealtimeMeshSectionConfig(
										  ERealtimeMeshSectionDrawType::Static, materialId),
									  true);

	SectionConfigs.Add(key, materialId);
}

void AChunkRmcActor::OnConstruction(const FTransform& Transform)
{
	RealtimeMeshComponent->InitializeRealtimeMesh<URealtimeMeshSimple>();

	defaultConfig = RealtimeMeshComponent->GetRealtimeMeshAs<
		URealtimeMeshSimple>()->GetCollisionConfig();
	
	
	Super::OnConstruction(Transform);
}
