#include "Spawner/Single/SingleChunkSpawner.h"

#include "Mesher/MeshingUtils/MesherVariables.h"
#include "Voxel/Generators/VoxelGeneratorBase.h"

void ASingleChunkSpawner::BeginPlay()
{
	UseWorldCenter = true;
	Super::BeginPlay();
}

void ASingleChunkSpawner::StartMeshing()
{
	FMesherVariables MeshVar;
	MeshVar.ChunkParams.SpawnerPtr = this;
	MeshVar.ChunkParams.OriginalChunk = SingleChunk;
	AddSideChunk(MeshVar, EFaceDirection::Top, nullptr);
	AddSideChunk(MeshVar, EFaceDirection::Bottom, nullptr);
	AddSideChunk(MeshVar, EFaceDirection::Front, nullptr);
	AddSideChunk(MeshVar, EFaceDirection::Back, nullptr);
	AddSideChunk(MeshVar, EFaceDirection::Right, nullptr);
	AddSideChunk(MeshVar, EFaceDirection::Left, nullptr);
	MeshVar.ChunkParams.ShowBorders = true;
	MeshVar.ChunkParams.WorldTransform = UseWorldCenter;
	VoxelGenerator->GenerateMesh(MeshVar);
}
