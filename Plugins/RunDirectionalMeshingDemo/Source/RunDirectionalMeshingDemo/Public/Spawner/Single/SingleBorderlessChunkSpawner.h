// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "SingleChunkSpawnerBase.h"
#include "Mesher/MeshingStructs/GridDirectionToFace.h"
#include "SingleBorderlessChunkSpawner.generated.h"
//TODO: add forward declarations

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RUNDIRECTIONALMESHINGDEMO_API ASingleBorderlessChunkSpawner : public ASingleChunkSpawnerBase
{
	GENERATED_BODY()
	
protected:
	virtual void StartMeshing() override;
	
private:
	TSharedPtr<FChunk> SideChunk[FACE_SIDE_COUNT];
	void SpawnSideChunk(FMesherVariables& chunkParams, const FGridDirectionToFace& faceDirection);
};
