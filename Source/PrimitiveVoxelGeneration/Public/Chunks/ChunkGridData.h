// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ChunkGridData.generated.h"

class UChunkBase;
struct FChunkSettings;
class AChunkActor;
/**
 * 
 */

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FChunkGridData 
{
	GENERATED_BODY()
	
	//TODO: naming conventions
	void AddChunkToGrid(UChunkBase* chunk, const FIntVector& gridPos) const;
	void RemoveChunkFromGrid(const FIntVector& gridPos) const;
	bool IsChunkInGrid(const FIntVector& gridPos) const;
	UChunkBase* GetChunkPtr(const FIntVector& gridPos) const;
	
	void SetSpawnedChunks(const TSharedPtr<TMap<FIntVector, UChunkBase*>>& spawnedChunks);
	void SetChunkSettings(const TSharedPtr<FChunkSettings>& chunkSettings);
	TSharedPtr<FChunkSettings> GetChunkSettings() const;
	
private:
	TSharedPtr<TMap<FIntVector, UChunkBase*>> SpawnedChunks;
	TSharedPtr<FChunkSettings> ChunkSettings;
};