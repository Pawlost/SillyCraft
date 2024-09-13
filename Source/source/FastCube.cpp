// Fill out your copyright notice in the Description page of Project Settings.
#include "FastCube.h"

// Sets default values
AFastCube::AFastCube() : m_mesh(CreateDefaultSubobject<UProceduralMeshComponent>("CustomMesh"))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(m_mesh);
}

// Called when the game starts or when spawned
void AFastCube::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFastCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFastCube::SetMesh(const int& index, const TArray<FVector>& vectors, const TArray<int32>& indice, const TArray<FLinearColor>& color)
{
	m_mesh->CreateMeshSection_LinearColor(index, vectors, indice, TArray<FVector>(), TArray<FVector2D>(), color, TArray<FProcMeshTangent>(), false);
	m_mesh->SetMeshSectionVisible(index, true);
	m_mesh->SetMaterial(index, m_material);
}

void AFastCube::Initialize(UMaterial* material)
{
	m_material = material;
}