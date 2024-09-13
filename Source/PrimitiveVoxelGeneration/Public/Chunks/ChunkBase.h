#pragma once

#include "CoreMinimal.h"
#include "GenerationSettings.h"
#include "ProceduralMeshComponent.h"
#include "UObject/Object.h"
#include "ChunkBase.generated.h"

/**
 
*/
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API UChunkBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void SetChunkSettings(const TSharedPtr<FGenerationSettings> ChunkSettings);

	virtual void GenerateMesh(UProceduralMeshComponent* procMesh){}
	virtual void GenerateVoxels(FVector& origin){}
	virtual int32 VoxelAt(int32 index){return 0;}

protected:
	TSharedPtr<FGenerationSettings> ChunkSettings = nullptr;
};