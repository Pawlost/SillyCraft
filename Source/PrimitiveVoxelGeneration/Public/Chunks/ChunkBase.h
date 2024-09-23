#pragma once

#include "CoreMinimal.h"
#include "ChunkActor.h"
#include "ProceduralMeshComponent.h"
#include "UObject/Object.h"
#include "Voxels/Voxel.h"
#include "ChunkBase.generated.h"

class UChunkGridData;
struct FChunkGridData;
struct FChunkSettings;
/**
 
*/
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API UChunkBase : public UObject
{
	GENERATED_BODY()

public:
	// Should be called immidietly after creation before spawning actor
	virtual void AddToGrid(const TWeakObjectPtr<UChunkGridData> chunkGridData, FIntVector& chunkGridPos);

	// should be called before generate mesh
	void StartSpawn(bool lockLocation = true);
	virtual void GenerateMesh(){}
	void RemoveMeshAsync() const;
	void RemoveMesh() const;
	
	// Will finish spawning with current mesh
	void FinishSpawn();

	void Despawn() const;
	
	virtual void GenerateVoxels(){}
	
	virtual FVoxel VoxelAt(int32 index){return FVoxel();}

	bool IsSpawned() const;

protected:
	TWeakObjectPtr<UChunkGridData> ChunkGridData;

	UPROPERTY()
	FTransform SpawnTransform = FTransform::Identity;
	
	UPROPERTY()
	TObjectPtr<AChunkActor> ChunkActor = nullptr;

	UPROPERTY()
	FIntVector ChunkGridPos;

	UPROPERTY()
	bool IsEmpty = true;

private:
	bool Spawned = false;
};