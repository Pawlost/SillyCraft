// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelGeneratorComponent.h"
UVoxelGeneratorComponent::UVoxelGeneratorComponent() : m_registry(std::make_shared<BlockRegistry>()), m_mesher(std::make_shared<ChunkMesher>(m_registry)), m_owner(GetOwner())
{
	PrimaryComponentTick.bCanEverTick = true;
}

//Defaults

// Called when the game starts
void UVoxelGeneratorComponent::BeginPlay()
{

	bool needspawn = false;
	Super::BeginPlay();

	m_save = Cast<USave>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	
	if(m_save == nullptr)
	{
		m_save = Cast<USave>(UGameplayStatics::CreateSaveGameObject(USave::StaticClass()));
		needspawn = true;
	}
	else 
	{
		m_owner->SetActorLocation(m_save->PlayerPosition);
	}

	for (const FPrimitiveChunk& chunk : m_save->SavedChunks)
	{
		m_savedChanges.Add(TTuple<int, int, int>(chunk.X, chunk.Y, chunk.Z), chunk);
	}

	m_lastPosition = m_owner->GetActorLocation();

	const int ownerX = m_lastPosition.X / Constants::ChunkLenght;
	const int ownerY = m_lastPosition.Y / Constants::ChunkLenght;
	const int ownerZ = m_lastPosition.Z / Constants::ChunkLenght;

	for (int x = ownerX - Constants::MeshZone - 1; x < ownerX + Constants::MeshZone + 1; x++)
	{
		for (int y = ownerY - Constants::MeshZone - 1; y < ownerY + Constants::MeshZone + 1; y++)
		{
			for (int z = ownerZ - Constants::MeshZone - 1; z < ownerZ + Constants::MeshZone + 1; z++)
			{
				FillZone(x, y, z, TTuple<int, int, int>(x, y, z));
			}
		}
	}

	ChangeZone(needspawn, m_lastPosition);
}

void UVoxelGeneratorComponent::EndPlay(const EEndPlayReason::Type)
{
	m_save->SavedChunks.Empty();
	for (TPair<TTuple<int, int, int>, FPrimitiveChunk> chunk : m_savedChanges)
	{
		m_save->SavedChunks.Add(chunk.Value);
	}

	UGameplayStatics::SaveGameToSlot(m_save, SlotName, UserIndex);
}

// Called every frame
void UVoxelGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FVector position = m_owner->GetActorLocation();

	if (OutZone(position.X, position.Y, position.Z, m_lastPosition.X, m_lastPosition.Y, m_lastPosition.Z, Constants::ChunkLenght))
	{
		m_lastPosition = position;
		ChangeZone(false, m_lastPosition);
	}
}


void UVoxelGeneratorComponent::ChangeZone(bool needspawn, const FVector& position)
{
	const int ownerX = position.X / Constants::ChunkLenght;
	const int ownerY = position.Y / Constants::ChunkLenght;
	const int ownerZ = position.Z / Constants::ChunkLenght;

	for (int x = ownerX - Constants::FillZone; x < ownerX + Constants::FillZone; x++)
	{
		for (int y = ownerY - Constants::FillZone; y < ownerY + Constants::FillZone; y++)
		{
			for (int z = ownerZ - Constants::FillZone; z < ownerZ + Constants::FillZone; z++)
			{
				const TTuple<int, int, int> pos(x, y, z);

				if (!m_chunks.Contains(pos)) {
					FillZone(x, y, z, pos);
				}

				if (InZone(x, y, z, ownerX, ownerY, ownerZ, Constants::MeshZone))
				{
					if (m_chunks.Contains(pos)) {
						AChunk* chunk = m_chunks[pos];
						if (!chunk->Generated) {
							const FVector chunkLocation = chunk->GetActorLocation();
							std::array<AChunk*, 6> sideChunks;
							GetChunkNeighbors(x, y, z, sideChunks);

							if (!m_mesher->MeshChunk(*chunk, sideChunks) && needspawn && chunkLocation.X == position.X && chunkLocation.Y == position.Y)
							{
								needspawn = false;
								m_owner->SetActorLocation(chunkLocation + Constants::ChunkScale);
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

	for (const TTuple<int, int, int>& key : keys)
	{
		AChunk* chunk = m_chunks[key];
		const FVector chunkLocation = chunk->GetActorLocation() / Constants::ChunkLenght;
		if (OutZone(chunkLocation.X, chunkLocation.Y, chunkLocation.Z, ownerX, ownerY, ownerZ, Constants::FillZone))
		{
			if (OutZone(chunkLocation.X, chunkLocation.Y, chunkLocation.Z, ownerX, ownerY, ownerZ, Constants::MeshZone))
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

void UVoxelGeneratorComponent::FillZone(const int& x, const int& y, const int& z, const TTuple<int, int, int>& pos)
{
	const int posX = x * Constants::ChunkLenght;
	const int posY = y * Constants::ChunkLenght;
	const int posZ = z * Constants::ChunkLenght;

	const FVector location(posX, posY, posZ);
	const FRotator rotation(0.0f, 0.0f, 0.0f);
	AChunk* chunk = GetWorld()->SpawnActor<AChunk>(location, rotation);
	chunk->Initialize(m_registry, Material);
	chunk->BaseFill();

	TArray<int> ids;
	m_registry->GetIDs(ids);

	for (const int& id : ids)
	{
		chunk->Fill(id);
	}

	if (m_savedChanges.Contains(pos))
	{
		const FPrimitiveChunk& primitiveChunk = m_savedChanges[pos];
		for (const TPair<int, int>& change : primitiveChunk.ChangeIds)
		{
			chunk->ChangeBlockID(change.Key, change.Value, true);
		}
	}

	m_chunks.Add(pos, chunk);
}

//Interaction
void UVoxelGeneratorComponent::Pick(const bool& hit, const FVector& hitLocation, const FVector& hitNormal)
{
	if (hit) {
		int x, y, z, chunkX, chunkY, chunkZ;

		FVector playerLocation = m_owner->GetActorLocation();

		CalculateHitCoords(x, y, z, chunkX, chunkY, chunkZ, hitLocation);

		if (hitNormal.X > 0)
		{
			x -= 1;
		}

		if (hitNormal.Y > 0)
		{
			y -= 1;
		}

		if (hitNormal.Z > 0)
		{
			z -= 1;
		}

		AChunk* chunk = m_chunks[TTuple<int, int, int>(chunkX, chunkY, chunkZ)];

		const int index = Constants::MakeIndex(y, z, x);
		const int id = chunk->GetBlockID(index);
		std::shared_ptr<Block> block = m_registry->GetBlock(id);

		if (m_damagedblock && block->ID == m_damagedblock->ID)
		{
			if (m_particles == nullptr)
			{
				const FRotator rotation(0.0f, 0.0f, 0.0f);
				const FActorSpawnParameters parameters;
				m_particles = GetWorld()->SpawnActor<AInteractionParticles>(hitLocation + (hitNormal * 20), rotation, parameters);

				m_particles->Initialize(Particles, block->Color);

				GetWorld()->GetTimerManager().SetTimer(m_timer, this, &UVoxelGeneratorComponent::DestroyParticles, 1.0f, false);
			}

			if (m_damagedblock->GetLifeSpan() < 0)
			{
				ChunkChanged(index, m_registry->AirID, *chunk);
				m_holdingblock = m_damagedblock;
				m_damagedblock = nullptr;
			}
			else
			{
				m_damagedblock->DecreaseLifeSpan(Constants::PickingSpeed);
			}
		}
		else
		{
			m_damagedblock = block;
			m_damagedblock->SetLifeSpan(m_damagedblock->BlockHardness * Constants::PickingMultiplier);
		}
	}
}

void UVoxelGeneratorComponent::Place(const bool& hit, const FVector& hitLocation, const FVector& normal)
{
	if (hit && m_highlightCube) {
		int x, y, z, chunkX, chunkY, chunkZ;

		GetWorld()->DestroyActor(m_highlightCube);
		m_highlightCube = nullptr;

		const FVector playerLocation = m_owner->GetActorLocation();

		CalculateHitCoords(x, y, z, chunkX, chunkY, chunkZ, hitLocation);

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
		const int index = Constants::MakeIndex(y, z, x);
		const int id = chunk->GetBlockID(index);

		if (id == m_registry->AirID) {
			ChunkChanged(index, m_holdingblock->ID, *chunk);
			m_holdingblock = nullptr;
		}
	}
}

void UVoxelGeneratorComponent::HighlightTargetBlock(const bool& hit, const FVector& hitLocation, const FVector& hitNormal)
{
	if (hit && m_holdingblock) {

		int x, y, z, chunkX, chunkY, chunkZ;

		FVector playerLocation = m_owner->GetActorLocation();

		CalculateHitCoords(x, y, z, chunkX, chunkY, chunkZ, hitLocation);

		if (hitNormal.X < 0)
		{
			x -= 1;
		}

		if (hitNormal.Y < 0)
		{
			y -= 1;
		}

		if (hitNormal.Z < 0)
		{
			z -= 1;
		}

		x *= Constants::ChunkScale + (chunkX * Constants::ChunkLenght);
		y *= Constants::ChunkScale + (chunkY * Constants::ChunkLenght);
		z *= Constants::ChunkScale + (chunkZ * Constants::ChunkLenght);

		FVector pos(x, y, z);

		if (OutZone(pos.X, pos.Y, pos.Z, playerLocation.X, playerLocation.Y, playerLocation.Z, Constants::ChunkScale))
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

void UVoxelGeneratorComponent::ChunkChanged(const int& index, const int& value, AChunk& chunk)
{
	chunk.ChangeBlockID(index, value, true);

	FVector chunkPos = chunk.GetActorLocation() / Constants::ChunkLenght;
	const int x = (int)chunkPos.X;
	const int y = (int)chunkPos.Y;
	const int z = (int)chunkPos.Z;

	std::array<AChunk*, 6> sideChunks;
	GetChunkNeighbors(x, y, z, sideChunks);

	m_mesher->MeshChunk(chunk, sideChunks);

	TTuple<int, int, int> pos(x, y, z);

	if (m_savedChanges.Contains(pos))
	{
		if (m_savedChanges[pos].ChangeIds.Contains(index))
		{
			m_savedChanges[pos].ChangeIds[index] = value;
		}
		else
		{
			m_savedChanges[pos].ChangeIds.Add(index, value);
		}
	}
	else 
	{
		FPrimitiveChunk primitiveChunk;
		primitiveChunk.X = x;
		primitiveChunk.Y = y;
		primitiveChunk.Z = z;
		primitiveChunk.ChangeIds.Add(index, value);
		m_savedChanges.Add(pos, primitiveChunk);
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


//Save
void UVoxelGeneratorComponent::SavePosition(const FVector& location, const FRotator& rotation)
{
	if (m_save)
	{
		m_save->PlayerPosition = location;
		m_save->PlayerRotation = rotation;
	}
}

FRotator UVoxelGeneratorComponent::GetSavedRotation() const
{
	if (m_save)
	{
		return m_save->PlayerRotation;
	}

	return FRotator(0.0f, 0.0f, 0.0f);
}

FVector UVoxelGeneratorComponent::GetSavedLocation() const
{
	if (m_save)
	{
		return m_save->PlayerPosition;
	}

	return FVector(0.0f, 0.0f, 0.0f);
}

//Utils
void UVoxelGeneratorComponent::GetChunkNeighbors(const int& x, const int& y, const int& z, std::array<AChunk*, 6>& outSideChunks) const
{
	outSideChunks[ChunkMesher::Faces::Front] = m_chunks[TTuple<int, int, int>(x - 1, y, z)],
	outSideChunks[ChunkMesher::Faces::Back] = m_chunks[TTuple<int, int, int>(x + 1, y, z)];
	outSideChunks[ChunkMesher::Faces::Right] = m_chunks[TTuple<int, int, int>(x, y + 1, z)];
	outSideChunks[ChunkMesher::Faces::Left] = m_chunks[TTuple<int, int, int>(x, y - 1, z)];
	outSideChunks[ChunkMesher::Faces::Bottom] = m_chunks[TTuple<int, int, int>(x, y, z - 1)];
	outSideChunks[ChunkMesher::Faces::Top] = m_chunks[TTuple<int, int, int>(x, y, z + 1)];
}

bool UVoxelGeneratorComponent::InZone(const int& posX, const int& posY, const int& posZ, const int& baseX, const int& baseY, const int& baseZ, const int& size) const
{
	return posX > baseX - size && posX < baseX + size &&
		posY > baseY - size && posY < baseY + size &&
		posZ > baseZ - size && posZ < baseZ + size;
}

bool UVoxelGeneratorComponent::OutZone(const int& posX, const int& posY, const int& posZ, const int& baseX, const int& baseY, const int& baseZ, const int& size) const
{
	return posX < baseX - size || posX > baseX + size ||
		posY < baseY - size || posY > baseY + size ||
		posZ < baseZ - size || posZ > baseZ + size;
}

void UVoxelGeneratorComponent::CalculateHitCoords(int& x, int& y, int& z, int& chunkX, int& chunkY, int& chunkZ, const FVector& hitLocation) const
{
	chunkX = hitLocation.X / Constants::ChunkLenght;
	chunkY = hitLocation.Y / Constants::ChunkLenght;
	chunkZ = hitLocation.Z / Constants::ChunkLenght;


	if (((int)hitLocation.X) < 0)
	{
		chunkX -= 1;
	}

	if (((int)hitLocation.Y) < 0)
	{
		chunkY -= 1;
	}

	if (((int)hitLocation.Z) < 0)
	{
		chunkZ -= 1;
	}

	x = abs(abs(hitLocation.X / Constants::ChunkScale) - abs(chunkX * Constants::ChunkSize));
	y = abs(abs(hitLocation.Y / Constants::ChunkScale) - abs(chunkY * Constants::ChunkSize));
	z = abs(abs(hitLocation.Z / Constants::ChunkScale) - abs(chunkZ * Constants::ChunkSize));
}
