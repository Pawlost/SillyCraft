// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "VoxelGeneratorBase.h"
#include "Components/ActorComponent.h"
#include "Chunks/ChunkStruct.h"
#include "SingleVoxelGenerator.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class PRIMITIVEVOXELGENERATION_API USingleVoxelGenerator : public UVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	virtual void GenerateVoxels(TSharedPtr<FChunkStruct>& chunk) override;
};
