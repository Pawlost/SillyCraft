#pragma once

#include "CoreMinimal.h"
#include "Voxel/Generators/VoxelGeneratorBase.h"
#include "SingleVoxelGeneratorBase.generated.h"

UCLASS(ClassGroup=(VoxelGeneration), Abstract, Blueprintable)
class RUNDIRECTIONALMESHINGDEMO_API USingleVoxelGeneratorBase : public UVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Voxels")
	FDataTableRowHandle VoxelTypeHandle;

protected:
	virtual void BeginPlay() override;
	FVoxel GetSingleVoxel() const;
	virtual TTuple<FName, FVoxelType> GetVoxelType(const FVoxel& Voxel) const override;
	virtual FVoxel GetVoxelByName(const FName& VoxelName) const override;
};
