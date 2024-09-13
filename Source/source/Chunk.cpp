// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk.h"

// Sets default values
AChunk::AChunk() : m_blockIDs(std::make_unique<std::array<FBlockID, Constants::ChunkSize3D>>()), m_noise(CreateDefaultSubobject<UFastNoiseWrapper>("Noise")), m_mesh(CreateDefaultSubobject<UProceduralMeshComponent>("CustomMesh")), m_material(CreateDefaultSubobject<UMaterial>("DefaultMaterial"))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(m_mesh);
	m_noise->SetupFastNoise(NoiseType, Seed, NoiseFrequency);
	m_mesh->bUseAsyncCooking = true;
	m_mesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
}

AChunk::~AChunk()
{
	DestroyConstructedComponents();
}

void AChunk::Initialize(std::shared_ptr<BlockRegistry> registry, UMaterial* material, const int& seed, const EFastNoise_NoiseType& noiseType, const float& noiseFrequency, const int& maxElevation)
{
	m_material = material;
	m_registry = registry;
	Seed = seed;
	NoiseType = noiseType;
	NoiseFrequency = noiseFrequency;
	MaxElevation = maxElevation;
}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AChunk::HasMesh() const
{
	return m_hasMesh;
}

void AChunk::SetMesh(const int& index, const TArray<FVector>& vectors, const TArray<int32>& indice, const TArray<FLinearColor>& color)
{
	m_mesh->CreateMeshSection_LinearColor(index, vectors, indice, TArray<FVector>(), TArray<FVector2D>(), color, TArray<FProcMeshTangent>(), true);
	m_mesh->SetMeshSectionVisible(index, true);
	m_mesh->SetMaterial(index, m_material);
}


// Called every frame
void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int AChunk::GetBlockID(const int& index) const
{
	return (*m_blockIDs)[index].BlockID;
}

bool AChunk::IsPlaceByPlayer(const int& index) const
{
	return (*m_blockIDs)[index].PlacedByPlayer;
}

void  AChunk::BaseFill()
{
	int origin, index;
	FVector actorLocation = GetActorLocation();
	int chunkX = actorLocation.X / Constants::ChunkScale;
	int chunkY = actorLocation.Y / Constants::ChunkScale;
	int chunkZ = actorLocation.Z / Constants::ChunkScale;

	for (int x = 0; x < Constants::ChunkSize; x++) 
	{
		for (int y = 0; y < Constants::ChunkSize; y++) 
		{
			origin = m_noise->GetNoise2D(y + chunkY, x + chunkX) * MaxElevation;
			for (int z = 0; z < Constants::ChunkSize; z++) 
			{
				index = Constants::MakeIndex(y, z, x);
				(*m_blockIDs)[index].PlacedByPlayer = false;
				if (chunkZ + z <= origin) 
				{
					(*m_blockIDs)[index].BlockID = m_registry->BaseBlockID;
						continue;
				}
				(*m_blockIDs)[index].BlockID = m_registry->AirID;
			}
		}
	}
}


void AChunk::Fill(const int& blockID)
{
	std::shared_ptr<Block> block = m_registry->GetBlock(blockID);
	if ((*block).BlockHardness != Block::Hardness::Empty)
	{
		int id, index;
		int seed = m_noise->GetSeed();
		m_noise->SetSeed(seed + blockID);

		const FVector& actorLocation = GetActorLocation();
		int chunkX = actorLocation.X / Constants::ChunkScale;
		int chunkY = actorLocation.Y / Constants::ChunkScale;
		int chunkZ = actorLocation.Z / Constants::ChunkScale;

		if (chunkZ >= (*block).MinElevation && chunkZ <= (*block).MaxElevation) {
			for (int x = 0; x < Constants::ChunkSize; x++) {
				for (int y = 0; y < Constants::ChunkSize; y++) {
					for (int z = 0; z < Constants::ChunkSize; z++) {
						index = Constants::MakeIndex(y, z, x);
						id = (*m_blockIDs)[index].BlockID;
						std::shared_ptr<Block> otherBlock = m_registry->GetBlock(id);
						if ((*otherBlock).BlockHardness != Block::Hardness::Empty && z < Constants::ChunkSize - 1)
						{
							id = (*m_blockIDs)[Constants::MakeIndex(y, z + 1, x)].BlockID;
							if (id == m_registry->AirID)
							{
								if (block->Range > 0) {
									int range = abs(m_noise->GetNoise2D(x, y)) * block->Range;
									for (int r = 0; r < range; r++)
										if (z - r > 0)
											(*m_blockIDs)[Constants::MakeIndex(y, z - r, x)].BlockID = block->ID;
								}
								else 
								{
									(*m_blockIDs)[index].BlockID = block->ID;
								}
							}
						}
					}
				}
			}
		}
		m_noise->SetSeed(seed);
	}
}

void AChunk::ChangeBlockID(const int& index, const int& id, bool changedByPlayer)
{
	(*m_blockIDs)[index].BlockID = id;
	(*m_blockIDs)[index].PlacedByPlayer = changedByPlayer;
}

void AChunk::Show()
{
	m_mesh->SetVisibility(true);
}

void AChunk::Hide() 
{
	m_mesh->SetVisibility(false);
}
