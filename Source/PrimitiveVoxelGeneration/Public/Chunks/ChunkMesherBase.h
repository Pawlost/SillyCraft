#pragma once

#include "CoreMinimal.h"
#include "RealtimeMeshActor.h"
#include "UObject/Object.h"
#include "Voxels/Voxel.h"
#include "ChunkMesherBase.generated.h"

class UChunkGridData;
struct FChunkGridData;
struct FChunkSettings;
/**
 
*/
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API UChunkMesherBase : public UActorComponent
{
	GENERATED_BODY()

public:
	// Should be called immidietly after creation before spawning actor
	virtual void AddToGrid(const TWeakObjectPtr<UChunkGridData> chunkGridData, FIntVector& chunkGridPos);

	// should be called before generate mesh
	void Spawn(bool lockLocation = true);
	virtual void GenerateMesh(){}
	void RemoveMeshAsync() const;
	void RemoveMesh() const;
	void Despawn() const;
	
	virtual void GenerateVoxels(){}
	
	virtual FVoxel VoxelAt(int32 index){return FVoxel();}
	
	bool IsEmpty() const;
	virtual double GetHighestElevationAtPosition(double posX, double posY);
	
protected:
	TWeakObjectPtr<UChunkGridData> ChunkGridData;

	UPROPERTY()
	FTransform SpawnTransform = FTransform::Identity;
	
	UPROPERTY()
	TObjectPtr<ARealtimeMeshActor> ChunkActor = nullptr;

	UPROPERTY()
	FIntVector ChunkGridPos;

	UPROPERTY()
	bool Empty = true;

private:
	mutable FCriticalSection DataGuard;
};