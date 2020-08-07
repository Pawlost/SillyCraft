// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"

// Sets default values
AChunk::AChunk() : m_noise(CreateDefaultSubobject<UFastNoiseWrapper>("Noise")), Mesh(CreateDefaultSubobject<UProceduralMeshComponent>("CustomMesh")), m_blockIDs(new std::array<int, Constants::ChunkSize3D>)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(Mesh);
	m_noise->SetupFastNoise(Constants::NoiseType, Constants::Seed, Constants::NoiseFrequency);
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

int AChunk::GetBlockID(const int& index) const
{

	return (*m_blockIDs)[index];
}

void  AChunk::BaseFill(int baseID, int airID)
{
	FVector actorLocation = GetActorLocation();
	int chunkX = actorLocation.X / Constants::ChunkScale;
	int chunkY = actorLocation.Y / Constants::ChunkScale;
	int chunkZ = actorLocation.Z / Constants::ChunkScale;

	for (int x = 0; x < Constants::ChunkSize; x++) 
	{
		for (int y = 0; y < Constants::ChunkSize; y++) 
		{
			int origin = m_noise->GetNoise2D(y + chunkY, x + chunkX) * Constants::MaxElevation;
			for (int z = 0; z < Constants::ChunkSize; z++) 
			{
				if (chunkZ + z <= origin) 
				{
						(*m_blockIDs)[y + z * Constants::ChunkSize + x * Constants::ChunkSize2D] = baseID;
						continue;
				}
				(*m_blockIDs)[y + z * Constants::ChunkSize + x * Constants::ChunkSize2D] = airID;
			}
		}
	}
}


void AChunk::Fill(const Block* block, const int& range)
{
	/*const FVector& actorLocation = GetActorLocation();
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
	}*/    
}

