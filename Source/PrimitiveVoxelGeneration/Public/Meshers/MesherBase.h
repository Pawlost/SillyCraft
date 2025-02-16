// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Voxels/ChunkStruct.h"
#include "MesherBase.generated.h"
//TODO: add forward declarations

#define FACE_COUNT 6

#define FRONT_FACE_INDEX 0
#define BACK_FACE_INDEX 1
#define RIGHT_FACE_INDEX 2
#define LEFT_FACE_INDEX 3
#define BOTTOM_FACE_INDEX 4
#define TOP_FACE_INDEX 5

UCLASS(Abstract)
class PRIMITIVEVOXELGENERATION_API UMesherBase : public UActorComponent
{
	GENERATED_BODY()

	virtual void GenerateMesh(FChunkStruct& chunk){};
};