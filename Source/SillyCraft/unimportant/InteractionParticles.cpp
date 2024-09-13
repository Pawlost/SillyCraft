// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionParticles.h"

// Sets default values
AInteractionParticles::AInteractionParticles() : m_particles(CreateDefaultSubobject<UParticleSystemComponent>("CustomParticles"))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(m_particles);
}

// Called when the game starts or when spawned
void AInteractionParticles::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractionParticles::Initialize(UParticleSystem* particles, const FLinearColor& color)
{
	m_particles->Template = particles;
	UMaterialInstanceDynamic* material = m_particles->CreateDynamicMaterialInstance(0);
	material->SetVectorParameterValue(TEXT("Color"), color);
	m_particles->ActivateSystem();
	m_particles->Activate();
}

// Called every frame
void AInteractionParticles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

