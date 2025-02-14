// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IVoxelGenerator.h"
#include "Voxels/VoxelType.h"

//TODO: specify class
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API UVoxelGeneratorBase : public UActorComponent, public IVoxelGenerator
{
	GENERATED_BODY()
	public:
		UVoxelGeneratorBase();
	
		int32 GetVoxelIndex(const int32 x, const int32 y, const int32 z) const;
		int32 GetVoxelCountY() const;
		int32 GetVoxelCountYZ() const;
		int32 GetVoxelCountXYZ() const;

		double_t GetMaximumElevation() const;

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Voxels")
    	TObjectPtr<UDataTable> VoxelTypeTable;

		int32 GetVoxelTypeCount() const;
		FVoxelType GetVoxelTypeById(const int32& voxelTypeIndex) const;

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Voxels")
		int32 VoxelCount = 0;

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Voxels")
		int32 VoxelSize = 0;
	
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Voxels")
		double_t MaxElevation = 0;

	private:
		int32 VoxelCountY = 0, VoxelCountYZ = 0, VoxelCountXYZ = 0;
	
};