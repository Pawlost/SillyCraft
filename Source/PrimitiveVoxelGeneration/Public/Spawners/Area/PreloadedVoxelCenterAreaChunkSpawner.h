#pragma once
#include "CoreMinimal.h"
#include "AreaChunkSpawnerBase.h"
#include "PreloadedVoxelCenterAreaChunkSpawner.generated.h"

//TODO: add forward declarations
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API APreloadedVoxelCenterAreaChunkSpawner : public AAreaChunkSpawnerBase
{
	GENERATED_BODY()

protected:
	virtual void GenerateArea() override;

private:
	
	// Called when the game starts
	virtual void BeginPlay() override;
	static void WaitForAllTasks(TArray<TSharedFuture<void>>& tasks);
};
