// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelGenerator.h"

// Sets default values
AVoxelGenerator::AVoxelGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
}
AVoxelGenerator::~AVoxelGenerator()
{
	//delete m_chunks;
}

// Called when the game starts or when spawned
void AVoxelGenerator::InitBlocks()
{

}

// Called when the game starts or when spawned
void AVoxelGenerator::BeginPlay()
{
	Super::BeginPlay();
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	Block* block = new Block("Stone", 1, Block::Hardness::Hard, FColor::Silver, 0, 123, false, 1.2f);
	m_blocks.Add(block);

	int index = 0;
	for(int x = 0; x < Constants::ChunkCount; x++)
	{
		for (int y = 0; y < Constants::ChunkCount; y++)
		{
			for (int z = 0; z < Constants::ChunkCount; z++)
			{
				const int& posX = x * Constants::ChunkLenght;
				const int& posY = y * Constants::ChunkLenght;
				const int& posZ = z * Constants::ChunkLenght;

				FVector Location(posX, posY, posZ);
				FRotator Rotation(0.0f, 0.0f, 0.0f);
				FActorSpawnParameters parameters;
				FString name("Chunk");
				name.AppendInt(int32(index));
				parameters.Name = FName(name);
				AChunk* chunk = GetWorld()->SpawnActor<AChunk>(Location, Rotation, parameters);
				m_ungeneratedChunks.Add(chunk);
				m_chunks.Add(TTuple<int, int, int>(x, y, z), chunk);
				chunk->Fill(block);
				index++;
			}
		}
	}

}

// Called every frame
void AVoxelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (m_ungeneratedChunks.Num() > 0) 
	{
		AChunk* chunk = m_ungeneratedChunks.Pop();
		ChunkMesher::MeshChunk(chunk);
		FString out("");
		out.AppendInt(m_ungeneratedChunks.Num());
		UE_LOG(LogTemp, Display, TEXT("Remaining chunks: %s"), *out);
	}
}

void AVoxelGenerator::Refill()
{
	for (Block* block : m_blocks)
	{
		for (TPair<TTuple<int, int, int>, AChunk*> pair : m_chunks)
		{
			AChunk* chunk = pair.Value;
			chunk->Fill(block);
		}
	}
}