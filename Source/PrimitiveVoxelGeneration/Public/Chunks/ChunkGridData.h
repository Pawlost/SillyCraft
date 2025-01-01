// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ChunkGridData.generated.h"

struct FVoxel;
struct FVoxelType;
class UChunkMesherBase;
struct FChunkSettings;
class AChunkActor;

UCLASS()
class PRIMITIVEVOXELGENERATION_API UChunkGridData : public UObject
{
	GENERATED_BODY()
	
public:
	//TODO: naming conventions
	void AddChunkToGrid(TWeakObjectPtr<UChunkMesherBase> chunk, const FIntVector& gridPos) const;
	void RemoveChunkFromGrid(const FIntVector& gridPos) const;
	bool IsChunkInGrid(const FIntVector& gridPos) const;
	TWeakObjectPtr<UChunkMesherBase> GetChunkPtr(const FIntVector& gridPos) const;
	
	void SetChunkSettings(const TSharedPtr<FChunkSettings>& chunkSettings);
	TSharedPtr<FChunkSettings> GetChunkSettings() const;

	TSharedPtr<TMap<FIntVector, TWeakObjectPtr<UChunkMesherBase>>> GetSpawnedChunks() const;
	void SetSpawnedChunks(const TSharedPtr<TMap<FIntVector, TWeakObjectPtr<UChunkMesherBase>>>& SpawnedChunks);
	void SetVoxelTypes(const TWeakObjectPtr<UDataTable>& VoxelTypes);
	int32 GetVoxelIdCount() const;
	
	FVoxelType GetVoxelTypeById(const int32& voxelId);
	
private:
	TSharedPtr<FChunkSettings> ChunkSettings;

	// chunks must have exact position in grid, this is the reason why scene tree cant be used
	TSharedPtr<TMap<FIntVector, TWeakObjectPtr<UChunkMesherBase>>> SpawnedChunks;
	TWeakObjectPtr<UDataTable> VoxelTypes;
	
	UPROPERTY()
	TArray<FName> VoxelIds;
	
	mutable FCriticalSection GridLock;
};