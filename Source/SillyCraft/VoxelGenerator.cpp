// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelGenerator.h"

// Sets default values
AVoxelGenerator::AVoxelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<UProceduralMeshComponent>("CustomMesh");

	mesh->SetupAttachment(RootComponent);
	TArray<FVector> vectors;
	TArray<int32> indice;
	vectors.Add(FVector(0, -100, 0));
	vectors.Add(FVector(0, -100, 100));

	vectors.Add(FVector(0, 100, 0));
	vectors.Add(FVector(0, 100, 100));

	vectors.Add(FVector(100, -100, 0));
	vectors.Add(FVector(100, -100, 100));

	vectors.Add(FVector(100, 100, 100));
	vectors.Add(FVector(100, 100, 0));

	TArray<FColor> color;
	color.Add(FColor(0.f, 0.f, 1.f));
	color.Add(FColor(0.f, 0.f, 1.f));
	color.Add(FColor(0.f, 0.f, 1.f));

	color.Add(FColor(0.f, 0.f, 1.f));
	color.Add(FColor(0.f, 0.f, 1.f));
	color.Add(FColor(0.f, 0.f, 1.f));

	color.Add(FColor(0.f, 0.f, 1.f));
	color.Add(FColor(0.f, 0.f, 1.f));
	color.Add(FColor(0.f, 0.f, 1.f));

	indice.Add(0);
	indice.Add(2);
	indice.Add(3);

	indice.Add(3);
	indice.Add(1);
	indice.Add(0);

	mesh->CreateMeshSection(int32(0), vectors, indice, TArray<FVector>(), TArray<FVector2D>(), color, TArray<FProcMeshTangent>(), true);
}

// Called when the game starts or when spawned
void AVoxelGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVoxelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

