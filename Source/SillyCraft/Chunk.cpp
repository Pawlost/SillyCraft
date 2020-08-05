// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"

// Sets default values
AChunk::AChunk() : m_blockIDs(new int[Constants::ChunkSize3D])
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("CustomMesh");
	Mesh->SetupAttachment(RootComponent);
}

AChunk::~AChunk()
{
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChunk::Fill(const Block& block)
{
	const FVector& actorLocation = GetActorLocation();
	for (int x = 0; x < Constants::ChunkSize; x++) {
		for (int z = 0; z < Constants::ChunkSize; z++) {
			int origin = block.Get(x + actorLocation.X * Constants::ChunkSize, z + actorLocation.Z * Constants::ChunkSize) - (Constants::ChunkSize * actorLocation.Y);
			for (int y = 0; y < block.Range; y++) {
				if (origin + y < Constants::ChunkSize) {
					m_blockIDs[x + (origin + y) * Constants::ChunkSize + z * Constants::ChunkSize * Constants::ChunkSize] = block.ID;
				}
			}
		}
	}
}

void AChunk::Fill(Block* block)
{
	const FVector& actorLocation = GetActorLocation();
	for (int x = 0; x < Constants::ChunkSize; x++) {
		for (int z = 0; z < Constants::ChunkSize; z++) {
			int origin = block->Get(x + actorLocation.X * Constants::ChunkSize, z + actorLocation.Z * Constants::ChunkSize) - (Constants::ChunkSize * actorLocation.Y);
			if (origin > 0) {
				for (int y = 0; y < block->Range; y++) {
					if (origin + y < Constants::ChunkSize) {
						m_blockIDs[x + (origin + y) * Constants::ChunkSize + z * Constants::ChunkSize * Constants::ChunkSize] = block->ID;
					}
				}
			}
		}
	}
}

