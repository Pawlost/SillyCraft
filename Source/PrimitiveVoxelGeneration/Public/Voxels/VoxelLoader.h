// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Meshers/MesherBase.h"
#include "VoxelLoader.generated.h"
struct FVoxelType;
using namespace UE::Math;
//TODO: add forward declarations

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class PRIMITIVEVOXELGENERATION_API AVoxelLoader : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation")
	TSubclassOf<UMesherBase> ChunkMesherTemplate = nullptr;
};
