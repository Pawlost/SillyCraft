// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelGeneratorComponent.h"


// Sets default values for this component's properties
UVoxelGeneratorComponent::UVoxelGeneratorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVoxelGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	
	auto location = GetOwner()->GetActorTransform().GetLocation();
	auto test = GetWorld()->SpawnActor(SpawnProperty);

	location.X += 10;
	test->SetActorLocation(location);
}


// Called every frame
void UVoxelGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto location = GetOwner()->GetActorTransform().GetLocation();

	GEngine->AddOnScreenDebugMessage(-1, 0.49f, FColor::Green,
	                                 *(FString::Printf(
		                                 TEXT("Stamina - X:%f | Y:%f | Z:%f"), location.X, location.Y, location.Z)));

	
	checkf(SpawnProperty,
	       TEXT("Unable to get reference to the EnhancedInputLocalPlayerSubsystem."));
}

