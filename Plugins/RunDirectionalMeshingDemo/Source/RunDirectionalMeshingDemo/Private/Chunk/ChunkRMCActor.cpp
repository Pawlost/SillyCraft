#include "Chunk/ChunkRMCActor.h"

#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"

AChunkRmcActor::AChunkRmcActor() : GroupKey(FRealtimeMeshSectionGroupKey::Create(0, FName("Chunk Mesh")))
{
	// Register and set RealTimeMeshComponent which will render and store generated mesh.
	RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RealtimeMeshComponent"));
	RealtimeMeshComponent->SetMobility(EComponentMobility::Movable);
	SetRootComponent(RealtimeMeshComponent);
}

void AChunkRmcActor::ClearMesh() const
{
	if (!IsValid(this))
	{
		return;
	}
	
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
	if (!IsValid(RealtimeMeshComponent))
	{
		return;
	}
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
