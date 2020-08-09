// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <ProceduralMeshComponent.h>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FastCube.generated.h"

UCLASS()
class SILLYCRAFT_API AFastCube : public AActor
{
	GENERATED_BODY()
private:
	UProceduralMeshComponent* m_mesh;	
	UMaterial* m_material;

public:	
	// Sets default values for this actor's properties
	AFastCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetMesh(const int& index, const TArray<FVector>& vectors, const TArray<int32>& indice, const TArray<FLinearColor>& color);
	void Initialize(UMaterial* material);

	void Activate();
	void Deactivate();
};
