// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "FastNoiseWrapper.h"
#include "Chunks/ChunkStruct.h"
#include "Components/ActorComponent.h"
#include "Voxels/VoxelType.h"
#include "Voxels/Interfaces/IVoxelGenerator.h"
#include "VoxelGeneratorBase.generated.h"

//TODO: specify class
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class PRIMITIVEVOXELGENERATION_API UVoxelGeneratorBase : public UActorComponent, public IVoxelGenerator
{
	GENERATED_BODY()

public:
	UVoxelGeneratorBase();

	int32 GetVoxelIndex(const int32 x, const int32 y, const int32 z) const;
	virtual double GetChunkSize() override;
	int32 GetVoxelDimensionCount() const;
	int32 GetVoxel2DimensionCount() const;
	int32 GetVoxel3DimensionCount() const;
	virtual double_t GetVoxelSize() override;

	double_t GetMaximumElevation() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Voxels")
	TObjectPtr<UDataTable> VoxelTypeTable;

	int32 GetVoxelTypeCount() const;
	FVoxelType GetVoxelTypeById(const int32& voxelTypeIndex) const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Voxels")
	int32 VoxelDimensionCount = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Voxels")
	double VoxelSize = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Voxels")
	double MaxElevation = 0;

	virtual double GetHighestElevationAtLocation(const FVector& location) override;

	int32 GetVoxelIndex(const FIntVector& indexVector) const;

	virtual void GenerateVoxels(TSharedPtr<FChunkStruct>& chunk) override
	{
	}

	static void AddVoxelAtIndex(const TSharedPtr<FChunkStruct>& chunk, const uint32& index, const FVoxel& voxel);

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> Noise;

	void SetupNoiseByVoxelId(int voxelId) const;

private:
	double ChunkSize = 0.0, InternalVoxelSize = 0.0;
	int32 VoxelCountY = 0, VoxelCountYZ = 0, VoxelCountXYZ = 0;
};
