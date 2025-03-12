﻿// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "SingleChunkSpawnerBase.h"
#include "Meshers/MeshingStructs/GridDirectionToFace.h"
#include "Chunks/ChunkStruct.h"
#include "SingleBorderlessChunkSpawner.generated.h"
//TODO: add forward declarations

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API ASingleBorderlessChunkSpawner : public ASingleChunkSpawnerBase
{
	GENERATED_BODY()
	
protected:
	virtual void StartMeshing() override;
	
private:
	TSharedPtr<FChunkStruct> SideChunk[FACE_SIDE_COUNT];
	void SpawnSideChunk(FChunkFaceParams& chunkParams, const FGridDirectionToFace& faceDirection);
};
