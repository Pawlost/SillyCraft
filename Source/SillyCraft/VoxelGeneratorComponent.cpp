// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelGeneratorComponent.h"

// Sets default values for this component's properties
UVoxelGeneratorComponent::UVoxelGeneratorComponent() :m_registry(new BlockRegistry()), m_mesher(new ChunkMesher(m_registry))
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
	for (int x = 0; x < Constants::MeshZone; x++)
	{
		for (int y = 0; y < Constants::MeshZone; y++)
		{
			for (int z = 0; z < Constants::MeshZone; z++)
			{
				const int& posX = x * Constants::ChunkLenght;
				const int& posY = y * Constants::ChunkLenght;
				const int& posZ = z * Constants::ChunkLenght;

				FVector location(posX, posY, posZ);
				FRotator rotation(0.0f, 0.0f, 0.0f);
				FActorSpawnParameters parameters;
				FString name("Chunk");
				name.AppendInt(int32(index));
				parameters.Name = FName(name);
				AChunk* chunk = GetWorld()->SpawnActor<AChunk>(location, rotation, parameters);
				m_chunks.Add(TTuple<int, int, int>(x, y, z), chunk);
				chunk->BaseFill(m_registry->GetBaseBlock(), m_registry->Air);
				m_ungeneratedChunks.Add(chunk);
				index++;
			}
		}
	}
    AActor* owner = GetOwner();
	FVector chunkLocation;
	bool needspawn = true;
	for (AChunk* chunk : m_ungeneratedChunks) 
	{
		chunkLocation = chunk->GetActorLocation();
		if (!GenerateChunk(*chunk) && needspawn)
		{
			needspawn = false;
			owner->SetActorLocation(chunkLocation);
		}
	}

	m_ungeneratedChunks.Empty();
}


// Called every frame
void UVoxelGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GenerateChunk();
}

bool UVoxelGeneratorComponent::GenerateChunk()
{
	if (m_ungeneratedChunks.Num() > 0)
	{
		AChunk* chunk = m_ungeneratedChunks.Pop();
		return GenerateChunk(*chunk);
	}

	return false;
}

bool UVoxelGeneratorComponent::GenerateChunk(AChunk& chunk)
{

	if (m_mesher->MeshChunk(chunk))
	{
		return true;
	}

	GetWorld()->DestroyActor(&chunk);

	return false;
}

void UVoxelGeneratorComponent::Refill()
{
	/*for (TPair<int, Block*> block : m_blocks)
	{
		for (TPair<TTuple<int, int, int>, AChunk*> pair : m_chunks)
		{
			AChunk* chunk = pair.Value;
			chunk->Fill(block.Value);
		}
	}*/
}

