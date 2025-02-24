#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Voxels/ChunkStruct.h"
#include "IVoxelGenerator.generated.h"

// UINTERFACE is used for the reflection system
UINTERFACE(MinimalAPI)
class UVoxelGenerator : public UInterface
{
	GENERATED_BODY()
};

// IInterface is the actual interface class
class PRIMITIVEVOXELGENERATION_API IVoxelGenerator
{
	GENERATED_BODY()

public:
	virtual void GenerateVoxels(TSharedPtr<FChunkStruct>& chunk) = 0;
	virtual double GetChunkSize() = 0;
	virtual double GetVoxelSize() = 0;
	virtual double GetHighestElevationAtLocation(const FVector& location) = 0;
};