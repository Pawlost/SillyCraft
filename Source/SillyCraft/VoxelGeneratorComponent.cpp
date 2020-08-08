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

	int index = 0;
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

/*for (TPair<int, Block*> block : m_blocks)
{
	for (TPair<TTuple<int, int, int>, AChunk*> pair : m_chunks)
	{
		AChunk* chunk = pair.Value;
		chunk->Fill(block.Value);
	}
}*/

void UVoxelGeneratorComponent::Refill()
{

}

