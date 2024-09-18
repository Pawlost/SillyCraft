#pragma once

#include "CoreMinimal.h"
#include "ChunkActor.h"
#include "ProceduralMeshComponent.h"
#include "UObject/Object.h"
#include "ChunkBase.generated.h"

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
	virtual void AddToGrid(const TSharedPtr<FChunkGridData> chunkGridData, FIntVector& chunkGridPos);

	// should be called before generate mesh
	void StartSpawn(bool lockLocation = true);
	virtual void GenerateMesh(){}
	void RemoveMeshAsync() const;
	void RemoveMesh() const;
	
	// Will finish spawning with current mesh
	void FinishSpawn();

	void Despawn() const;
	
	virtual void GenerateVoxels(){}
	
	virtual int32 VoxelAt(int32 index){return 0;}

	bool IsSpawned() const;

protected:
	TSharedPtr<FChunkGridData> ChunkGridData = nullptr;

	UPROPERTY()
	FTransform SpawnTransform = FTransform::Identity;
	
	UPROPERTY()
	TObjectPtr<AChunkActor> ChunkActor = nullptr;

	UPROPERTY()
	FIntVector ChunkGridPos;

private:
	bool Spawned = false;
};