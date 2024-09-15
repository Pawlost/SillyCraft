#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "UObject/Object.h"
#include "ChunkBase.generated.h"

struct FChunkGridData;
struct FChunkSettings;
/**
 
*/
UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API UChunkBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void SetChunkGridData(const TSharedPtr<FChunkGridData> chunkGridData);

	virtual void GenerateMesh(UProceduralMeshComponent* procMesh, FIntVector& chunkGridPos){}
	virtual void GenerateVoxels(FIntVector& chunkGridPos){}
	virtual int32 VoxelAt(int32 index){return 0;}

protected:
	TSharedPtr<FChunkGridData> ChunkGridData = nullptr;
};