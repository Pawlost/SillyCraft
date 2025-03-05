// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/SingleBorderlessChunkSpawner.h"

#include "Meshers/MeshingStructs/GridDirectionToFace.h"
#include "Spawners/SingleChunkSpawner.h"

void ASingleBorderlessChunkSpawner::ModifyVoxelAtChunk(const FIntVector& chunkGridPosition,
	const FIntVector& voxelPosition, const FVoxel& VoxelId)
{
	VoxelGenerator->ChangeVoxelIdInChunk(SingleChunk, voxelPosition, FVoxel(VoxelId));
}

void ASingleBorderlessChunkSpawner::BeginPlay()
{
	Super::BeginPlay();
	AsyncTask(ENamedThreads::AnyThread, [this]()
	{
		SingleChunk = MakeShared<FChunkStruct>();
		InitChunk(SingleChunk, SingleChunkGridPosition);
		SingleChunk->ChunkMeshActor = SpawnChunkActor(SingleChunk->GridPosition, nullptr).Get();
		ChunkParams.ChunkParams.OriginalChunk = SingleChunk;

		SpawnSideChunk(FGridDirectionToFace::TopDirection);
		SpawnSideChunk(FGridDirectionToFace::BottomDirection);
		SpawnSideChunk(FGridDirectionToFace::BackDirection);
		SpawnSideChunk(FGridDirectionToFace::FrontDirection);
		SpawnSideChunk(FGridDirectionToFace::RightDirection);
		SpawnSideChunk(FGridDirectionToFace::LeftDirection);

		VoxelGenerator->GenerateMesh(ChunkParams);
	});
}

void ASingleBorderlessChunkSpawner::SpawnSideChunk(const FGridDirectionToFace& faceDirection)
{
	auto index = static_cast<uint8>(faceDirection.FaceSide);
	auto chunk = MakeShared<FChunkStruct>().ToSharedPtr();
	SideChunk[index] = chunk;
	auto gridPosition = SingleChunkGridPosition + faceDirection.Direction;
	InitChunk(chunk, gridPosition);
	AddSideChunk(ChunkParams, faceDirection.FaceSide, SideChunk[index]);
}
