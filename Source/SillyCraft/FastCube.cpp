// Fill out your copyright notice in the Description page of Project Settings.


#include "FastCube.h"

// Sets default values
AFastCube::AFastCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

