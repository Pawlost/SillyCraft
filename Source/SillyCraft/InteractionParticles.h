// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Actor.h"
#include "InteractionParticles.generated.h"

UCLASS()
class SILLYCRAFT_API AInteractionParticles : public AActor
{
	GENERATED_BODY()
private:
	UParticleSystemComponent* m_particles;
public:	
	// Sets default values for this actor's properties
	AInteractionParticles();

	void Initialize(UParticleSystem* particles, const FLinearColor& color);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
