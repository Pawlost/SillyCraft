#pragma once

#include "CoreMinimal.h"
#include "Voxel.generated.h"

USTRUCT()
struct FVoxel
{
	GENERATED_BODY()

	int32 VoxelId;
};

USTRUCT()
struct PRIMITIVEVOXELGENERATION_API FVoxelFace
{
	GENERATED_BODY()

	FVector BeginVertexDown;
	FVector BeginVertexUp;
	FVector EndVertexDown;
	FVector EndVertexUp;
};