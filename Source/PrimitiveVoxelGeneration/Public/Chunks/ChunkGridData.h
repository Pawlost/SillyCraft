// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkBase.h"
#include "UObject/Object.h"
#include "ChunkGridData.generated.h"

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
	void AddChunkToGrid(const FIntVector& gridPos, AChunkActor* actor) const;
	void RemoveChunkFromGrid(const FIntVector& gridPos) const;
	bool IsChunkInGrid(const FIntVector& gridPos) const;
	void SetSpawnedChunks(const TSharedPtr<TMap<FIntVector, AChunkActor*>>& spawnedChunks);
	TSubclassOf<UChunkBase> GetChunkClass() const;
	void SetChunkClass(const TSubclassOf<UChunkBase>& chunkClass);
	
	void SetChunkSettings(const TSharedPtr<FChunkSettings>& chunKSettings);
	TSharedPtr<FChunkSettings> GetChunkSettings() const;
	
private:
	UPROPERTY()
	TSubclassOf<UChunkBase> ChunkClass = nullptr;
	
	TSharedPtr<TMap<FIntVector, AChunkActor*>> SpawnedChunks;
	TSharedPtr<FChunkSettings> ChunkSettings;

	//mutable FCriticalSection DataGuard;
};