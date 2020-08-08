// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelGeneratorComponent.h"

// Sets default values for this component's properties
UVoxelGeneratorComponent::UVoxelGeneratorComponent() : m_registry(new BlockRegistry()), m_mesher(new ChunkMesher(m_registry)), m_owner(GetOwner())
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UVoxelGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	m_lastPosition = m_owner->GetActorLocation();
	ChangeZone(true);
}



void UVoxelGeneratorComponent::ChangeZone(bool needspawn)
{
	FVector ownerLocation = m_owner->GetActorLocation();
	int ownerX = ownerLocation.X / Constants::ChunkLenght;
	int ownerY = ownerLocation.Y / Constants::ChunkLenght;
	int ownerZ = ownerLocation.Z / Constants::ChunkLenght;

	for (int x = ownerX - Constants::FillZone; x < ownerX + Constants::FillZone; x++)
	{
		for (int y = ownerY - Constants::FillZone; y < ownerY + Constants::FillZone; y++)
		{
			for (int z = ownerZ - Constants::FillZone; z < ownerZ + Constants::FillZone; z++)
			{
				const int& posX = x * Constants::ChunkLenght;
				const int& posY = y * Constants::ChunkLenght;
				const int& posZ = z * Constants::ChunkLenght;

				if (!m_chunks.Contains(TTuple<int, int, int>(x, y, z))) {
					FVector location(posX, posY, posZ);
					FRotator rotation(0.0f, 0.0f, 0.0f);
					FActorSpawnParameters parameters;
					FString name("Chunk: x ");
					name.AppendInt(x);
					name.Append(" y ");
					name.AppendInt(y);
					name.Append(" z ");
					name.AppendInt(z);

					parameters.Name = FName(name);
					AChunk* chunk = GetWorld()->SpawnActor<AChunk>(location, rotation, parameters);
					chunk->Initialize(m_registry, Material);
					chunk->BaseFill();
					TArray<int> ids;
					m_registry->GetIDs(ids);
					for (int id : ids)
					{
						chunk->Fill(id);
					}
					m_chunks.Add(TTuple<int, int, int>(x, y, z), chunk);
				}
			}
		}
	}

	for (int x = ownerX - Constants::MeshZone; x < ownerX + Constants::MeshZone; x++)
	{
		for (int y = ownerY - Constants::MeshZone; y < ownerY + Constants::MeshZone; y++)
		{
			for (int z = ownerZ - Constants::MeshZone; z < ownerZ + Constants::MeshZone; z++)
			{

				if (m_chunks.Contains(TTuple<int, int, int>(x, y, z))) {
					AChunk* chunk = m_chunks[TTuple<int, int, int>(x, y, z)];
					if (chunk && !chunk->Generated && chunk->GetRootComponent()) {
						FVector location = chunk->GetActorLocation();
						if (!m_mesher->MeshChunk(*chunk)) {
							if (needspawn && location.X == ownerLocation.X && location.Y == ownerLocation.Y)
							{
								location.X += Constants::ChunkScale;
								location.Y += Constants::ChunkScale;
								location.Z += Constants::ChunkScale;
								needspawn = false;
								m_owner->SetActorLocation(location);
							}
						}
						chunk->Generated = true;
					}
				}
			}
		}
	}
}
// Called every frame
void UVoxelGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector position = m_owner->GetActorLocation();

	if (m_lastPosition != position) 
	{
		m_lastPosition = position;
		ChangeZone(false);
	}
}

void UVoxelGeneratorComponent::Pick(const bool& hit, FVector location, const FVector& normal)
{
	if (hit) {
		int x = location.X;
		int y = location.Y;
		int z = location.Z;

		if (x <= 0) 
		{
			x = x - Constants::ChunkLenght;
		}

		if (y <= 0)
		{
			y = y - Constants::ChunkLenght;
		}

		if (z <= 0)
		{
			z = z - Constants::ChunkLenght;
		}

		x = x / Constants::ChunkLenght;
		y = y / Constants::ChunkLenght;
		z = z / Constants::ChunkLenght;

		AChunk* chunk = m_chunks[TTuple<int, int, int>(x, y, z)];

		x = abs(abs(location.X / Constants::ChunkScale) - abs(x * Constants::ChunkSize));
	    y = abs(abs(location.Y / Constants::ChunkScale) - abs(y * Constants::ChunkSize));
		z = abs(abs(location.Z / Constants::ChunkScale) - abs(z * Constants::ChunkSize));

		if (normal.X > 0 )
		{
			x -= 1;
		}


		if (normal.Y > 0)
		{
			y -= 1;
		}

		if (normal.Z > 0)
		{
			z -= 1;
		}

		int index = Constants::MakeIndex(y, z, x);
		int id = chunk->GetBlockID(index);
		Block* block = m_registry->GetBlock(id);

		FRotator rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters parameters;
		if (m_particles == nullptr) {
			m_particles = GetWorld()->SpawnActor<AInteractionParticles>(location + (normal * 20), rotation, parameters);

			m_particles->Initialize(Particles, block->Color);

			GetWorld()->GetTimerManager().SetTimer(m_timer, this, &UVoxelGeneratorComponent::DestroyParticles, 1.0f, false);
		}

		chunk->ChangeBlockID(index, m_registry->AirID);
		m_mesher->MeshChunk(*chunk);
	}
}

void UVoxelGeneratorComponent::Place(const bool& hit, const FVector& location, const FVector& normal)
{
	if (hit) {

	}
}

void UVoxelGeneratorComponent::DestroyParticles() 
{
	GetWorld()->DestroyActor(m_particles);
	m_particles = nullptr;
}

void UVoxelGeneratorComponent::Refill()
{

}

