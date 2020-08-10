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
	m_save = Cast<USave>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	
	if(m_save == nullptr)
	{
		m_save = Cast<USave>(UGameplayStatics::CreateSaveGameObject(USave::StaticClass()));
	}

	m_owner->SetActorLocation(m_save->PlayerPosition);

	m_save->Init();
	m_lastPosition = m_owner->GetActorLocation();
	ChangeZone(true);
}

void UVoxelGeneratorComponent::EndPlay(const EEndPlayReason::Type)
{
	m_save->Deinit();
	UGameplayStatics::SaveGameToSlot(m_save, SlotName, UserIndex);
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

				TTuple<int, int, int> pos(x, y, z);

				if (!m_chunks.Contains(pos)) {
					FVector location(posX, posY, posZ);
					FRotator rotation(0.0f, 0.0f, 0.0f);
					AChunk* chunk = GetWorld()->SpawnActor<AChunk>(location, rotation);
					chunk->Initialize(m_registry, Material);
					chunk->BaseFill();
					TArray<int> ids;
					m_registry->GetIDs(ids);
					for (int id : ids)
					{
						chunk->Fill(id);
					}
					if (m_save->ChunkMap.Contains(pos))
					{
						FPrimitiveChunk& primitiveChunk = m_save->ChunkMap[pos];
						for (TPair<int, int> change : primitiveChunk.ChangeIds)
						{
							chunk->ChangeBlockID(change.Key, change.Value);
						}
					}

					m_chunks.Add(pos, chunk);
				}

				if ( x > ownerX - Constants::MeshZone &&  x < ownerX + Constants::MeshZone &&
					 y > ownerY - Constants::MeshZone &&  y < ownerY + Constants::MeshZone &&
					 z > ownerZ - Constants::MeshZone &&  z < ownerZ + Constants::MeshZone)
				{
					if (m_chunks.Contains(TTuple<int, int, int>(x, y, z))) {
						AChunk* chunk = m_chunks[TTuple<int, int, int>(x, y, z)];
						if (!chunk->Generated) {
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
						chunk->Show();
					}
				}
			}
		}
	}

	TArray<TTuple<int, int, int>> keys;
	m_chunks.GetKeys(keys);

	for (TTuple<int, int, int> key : keys)
	{
		AChunk* chunk = m_chunks[key];
		FVector vector = chunk->GetActorLocation() / Constants::ChunkLenght;
		if (vector.X < ownerX - Constants::MeshZone || vector.X > ownerX + Constants::MeshZone ||
			vector.Y < ownerY - Constants::MeshZone || vector.Y > ownerY + Constants::MeshZone ||
			vector.Z < ownerZ - Constants::MeshZone || vector.Z > ownerZ + Constants::MeshZone)
		{
			if (vector.X < ownerX - Constants::FillZone || vector.X > ownerX + Constants::FillZone ||
				vector.Y < ownerY - Constants::FillZone || vector.Y > ownerY + Constants::FillZone ||
				vector.Z < ownerZ - Constants::FillZone || vector.Z > ownerZ + Constants::FillZone)
			{
				if (GetWorld()->DestroyActor(chunk)) 
				{
					m_chunks.Remove(key);
				}
			}

			chunk->Hide();
		}
	}
}
// Called every frame
void UVoxelGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector position = m_owner->GetActorLocation();
	FRotator rotator = m_owner->GetActorRotation();

	if (m_save) 
	{
		m_save->PlayerPosition = position;
		m_save->PlayerRotation = rotator;
	}

	if (m_lastPosition != position) 
	{
		m_lastPosition = position;
		ChangeZone(false);
	}
}

void UVoxelGeneratorComponent::Pick(const bool& hit, FVector location, const FVector& normal)
{
	if (hit) {
		int chunkX = location.X / Constants::ChunkLenght;
		int chunkY = location.Y / Constants::ChunkLenght;
		int chunkZ = location.Z / Constants::ChunkLenght;

		if (((int)location.X) < 0)
		{
			chunkX -= 1;
		}

		if (((int)location.Y) < 0)
		{
			chunkY -= 1;
		}

		if (((int)location.Z) < 0)
		{
			chunkZ -= 1;
		}

		int x = abs(location.X / Constants::ChunkScale - chunkX * Constants::ChunkSize);
		int y = abs(location.Y / Constants::ChunkScale - chunkY * Constants::ChunkSize);
		int z = abs(location.Z / Constants::ChunkScale - chunkZ * Constants::ChunkSize);

		if (normal.X > 0)
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

		AChunk* chunk = m_chunks[TTuple<int, int, int>(chunkX, chunkY, chunkZ)];

		int index = Constants::MakeIndex(y, z, x);
		int id = chunk->GetBlockID(index);
		Block* block = m_registry->GetBlock(id);

		m_mutex.lock();

		if (m_damagedblock && block == m_damagedblock)
		{

			FRotator rotation(0.0f, 0.0f, 0.0f);
			FActorSpawnParameters parameters;
			if (m_particles == nullptr)
			{
				m_particles = GetWorld()->SpawnActor<AInteractionParticles>(location + (normal * 20), rotation, parameters);

				m_particles->Initialize(Particles, block->Color);

				GetWorld()->GetTimerManager().SetTimer(m_timer, this, &UVoxelGeneratorComponent::DestroyParticles, 1.0f, false);
			}

			if (m_damagedblock->LifeSpan < 0)
			{
				ChunkChanged(index, m_registry->AirID, chunk);
				m_holdingblock = m_damagedblock;
				m_damagedblock = nullptr;
			}
			else
			{
				m_damagedblock->LifeSpan -= Constants::PickingSpeed;
			}
		}
		else
		{
			m_damagedblock = block;
			m_damagedblock->LifeSpan = m_damagedblock->BlockHardness * Constants::PickingMultiplier;
		}

		m_mutex.unlock();
	}
}

void UVoxelGeneratorComponent::Place(const bool& hit, const FVector& location, const FVector& normal)
{
	if (hit && m_highlightCube) {
		GetWorld()->DestroyActor(m_highlightCube);
		m_highlightCube = nullptr;

		int chunkX = location.X / Constants::ChunkLenght;
		int chunkY = location.Y / Constants::ChunkLenght;
		int chunkZ = location.Z / Constants::ChunkLenght;

		if (((int)location.X) < 0)
		{
			chunkX -= 1;
		}

		if (((int)location.Y) < 0)
		{
			chunkY -= 1;
		}

		if (((int)location.Z) < 0)
		{
			chunkZ -= 1;
		}

		int x = abs(location.X / Constants::ChunkScale - chunkX * Constants::ChunkSize);
		int y = abs(location.Y / Constants::ChunkScale - chunkY * Constants::ChunkSize);
		int z = abs(location.Z / Constants::ChunkScale - chunkZ * Constants::ChunkSize);

		if (normal.X < 0)
		{
			x -= 1;
		}

		if (normal.Y < 0)
		{
			y -= 1;
		}

		if (normal.Z < 0)
		{
			z -= 1;
		}

		AChunk* chunk = m_chunks[TTuple<int, int, int>(chunkX, chunkY, chunkZ)];
		int index = Constants::MakeIndex(y, z, x);
		int id = chunk->GetBlockID(index);

		if (id == m_registry->AirID) {
			//setblockid
			ChunkChanged(index, m_holdingblock->ID, chunk);
			m_holdingblock = nullptr;
		}
	}
}

void UVoxelGeneratorComponent::HighlightTargetBlock(const bool& hit, FVector location, const FVector& normal)
{
	if (hit && m_holdingblock) {
		FVector playerLocation = m_owner->GetActorLocation();

		int chunkX = location.X / Constants::ChunkLenght;
		int chunkY = location.Y / Constants::ChunkLenght;
		int chunkZ = location.Z / Constants::ChunkLenght;


		if (((int)location.X) < 0)
		{
			chunkX -= 1;
		}

		if (((int)location.Y) < 0)
		{
			chunkY -= 1;
		}

		if (((int)location.Z) < 0)
		{
			chunkZ -= 1;
		}

		int x = abs(location.X / Constants::ChunkScale - chunkX * Constants::ChunkSize);
		int y = abs(location.Y / Constants::ChunkScale - chunkY * Constants::ChunkSize);
		int z = abs(location.Z / Constants::ChunkScale - chunkZ * Constants::ChunkSize);

		if (normal.X < 0)
		{
			x -= 1;
		}

		if (normal.Y < 0)
		{
			y -= 1;
		}

		if (normal.Z < 0)
		{
			z -= 1;
		}

		playerLocation = FVector((int)(playerLocation.X / Constants::ChunkScale) * Constants::ChunkScale, (int)(playerLocation.Y / Constants::ChunkScale) * Constants::ChunkScale, (int)(playerLocation.Z / Constants::ChunkScale) * Constants::ChunkScale);

		FVector pos(x * Constants::ChunkScale + chunkX * Constants::ChunkLenght, y * Constants::ChunkScale + chunkY * Constants::ChunkLenght, z * Constants::ChunkScale + chunkZ * Constants::ChunkLenght);

		if (pos.X < playerLocation.X - Constants::ChunkScale || pos.X > playerLocation.X + Constants::ChunkScale ||
			pos.Y < playerLocation.Y - Constants::ChunkScale || pos.Y > playerLocation.Y + Constants::ChunkScale ||
			pos.Z < playerLocation.Z - Constants::ChunkScale || pos.Z > playerLocation.Z + Constants::ChunkScale)
		{
			if (m_highlightCube)
			{
				m_highlightCube->SetActorLocation(pos);
			}
			else
			{
				m_highlightCube = GetWorld()->SpawnActor<AFastCube>(pos, FRotator(0, 0, 0));
				m_highlightCube->Initialize(Material);
				m_mesher->CreateFastCube(*m_highlightCube, m_holdingblock->Color);
			}

			return;
		}
	}

	if (m_highlightCube)
	{
		GetWorld()->DestroyActor(m_highlightCube);
		m_highlightCube = nullptr;
	}
}

void UVoxelGeneratorComponent::ChunkChanged(int index, int value, AChunk* chunk) 
{
	chunk->ChangeBlockID(index, value);
	m_mesher->MeshChunk(*chunk);
	FVector chunkPos = chunk->GetActorLocation() / Constants::ChunkLenght;
	int x = (int)chunkPos.X;
	int y = (int)chunkPos.Y;
	int z = (int)chunkPos.Z;

	TTuple<int, int, int> pos(x, y, z);

	if (m_save->ChunkMap.Contains(pos))
	{
		FPrimitiveChunk& primitiveChunk = m_save->ChunkMap[pos];

		if (primitiveChunk.ChangeIds.Contains(index))
		{
			primitiveChunk.ChangeIds.Remove(index);
		}

		primitiveChunk.ChangeIds.Add(index, value);
	}
	else 
	{
		FPrimitiveChunk primitiveChunk;
		primitiveChunk.X = x;
		primitiveChunk.Y = y;
		primitiveChunk.Z = z;
		primitiveChunk.ChangeIds.Add(index, value);
		m_save->ChunkMap.Add(pos, primitiveChunk);
		m_save->SavedChunks.Add(primitiveChunk);
	}
}

void UVoxelGeneratorComponent::DestroyParticles() 
{
	if (m_particles)
	{
		GetWorld()->DestroyActor(m_particles);
		m_particles = nullptr;
	}
}	

FRotator UVoxelGeneratorComponent::GetSavedRotation()
{
	if (m_save) 
	{
		return m_save->PlayerRotation;
	}

	return FRotator();
}