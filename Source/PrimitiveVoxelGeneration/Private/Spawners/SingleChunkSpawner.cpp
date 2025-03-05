// Fill out your copyright notice in the Description page of Project Settings.pp[p
#include "Spawners/SingleChunkSpawner.h"

void ASingleChunkSpawner::ModifyVoxelAtChunk(const FIntVector& chunkGridPosition, const FIntVector& voxelPosition,
	const FVoxel& VoxelId)
{
	VoxelGenerator->ChangeVoxelIdInChunk(SingleChunk, voxelPosition, FVoxel(VoxelId));
	StartMeshing();
}

void ASingleChunkSpawner::BeginPlay()
{
	Super::BeginPlay();
	SingleChunk = MakeShared<FChunkStruct>();

	AsyncTask(ENamedThreads::AnyThread, [this]()
	{
		InitChunk(SingleChunk, SingleChunkGridPosition);
		SingleChunk->ChunkMeshActor = SpawnChunkActor(SingleChunk->GridPosition, nullptr).Get();
		StartMeshing();
	});
}

void ASingleChunkSpawner::StartMeshing() const
{
	FChunkFaceParams params;
	params.ChunkParams.OriginalChunk = SingleChunk;
	AddSideChunk(params, EFaceDirection::Top, nullptr);
	AddSideChunk(params, EFaceDirection::Bottom, nullptr);
	AddSideChunk(params, EFaceDirection::Front, nullptr);
	AddSideChunk(params, EFaceDirection::Back, nullptr);
	AddSideChunk(params, EFaceDirection::Right, nullptr);
	AddSideChunk(params, EFaceDirection::Left, nullptr);
	VoxelGenerator->GenerateMesh(params);
}
