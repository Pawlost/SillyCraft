// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "VoxelGeneratorBase.h"
#include "Components/ActorComponent.h"
#include "Chunks/ChunkStruct.h"
#include "Structs/NoiseGeneratorVariables.h"
#include "NoiseVoxelGridGenerator.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class PRIMITIVEVOXELGENERATION_API UNoiseVoxelGridGenerator : public UVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	int32 GetVoxelTypeCount() const;
	virtual void GenerateVoxels(FChunkStruct& chunk) override;
	virtual double GetHighestElevationAtLocation(const FVector& location) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Voxels")
	TObjectPtr<UDataTable> VoxelTypeTable;
	virtual TTuple<FName, FVoxelType> GetVoxelTypeById(const int32& voxelTypeIndex) const override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TArray<FNoiseGeneratorVariables> NoiseGenerators;
};
