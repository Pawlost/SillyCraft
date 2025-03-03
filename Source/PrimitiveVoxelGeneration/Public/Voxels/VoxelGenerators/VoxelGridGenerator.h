// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "NoiseVoxelGridGenerator.h"
#include "Components/ActorComponent.h"
#include "Chunks/ChunkStruct.h"
#include "VoxelGridGenerator.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class PRIMITIVEVOXELGENERATION_API UVoxelGridGenerator : public UVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxels")
	FDataTableRowHandle RowHandle;

	virtual void GenerateVoxels(TSharedPtr<FChunkStruct>& chunk) override;
};
