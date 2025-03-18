#pragma once
#include "CoreMinimal.h"
#include "SingleChunkSpawnerBase.h"
#include "Mesher/MeshingUtils/FaceDirection.h"
#include "SingleBorderlessChunkSpawner.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RUNDIRECTIONALMESHINGDEMO_API ASingleBorderlessChunkSpawner : public ASingleChunkSpawnerBase
{
	GENERATED_BODY()
	
protected:
	virtual void StartMeshing() override;
	
private:
	TSharedPtr<FChunk> SideChunk[CHUNK_FACE_COUNT];
	void SpawnSideChunk(FMesherVariables& MeshVars, const FFaceToDirection& FaceDirection);
};
