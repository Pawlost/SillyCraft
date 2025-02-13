// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IVoxelGenerator.h"

//TODO: specify class
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRIMITIVEVOXELGENERATION_API UVoxelGeneratorBase : public UActorComponent, public IVoxelGenerator
{
	GENERATED_BODY()
	public:
		
};