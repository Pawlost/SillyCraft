// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.generated.h"

USTRUCT(BlueprintType)
struct SILLYCRAFT_API FBlock
{
	GENERATED_USTRUCT_BODY()
public:
	static enum Hardness {
		Empty, Soft, Medium, Hard
	};

	FName Name;
	Hardness BlockHardness;
	FLinearColor Color;
	int Range;
	int MinElevation;
	int MaxElevation;
	int ID;
	int LifeSpan;
	bool null;
	
	FBlock() 
	{
		null = true;
	}
	
	FBlock(const FName& name, const int& id, Hardness blockHardness, FLinearColor color, int range, int maxElevation, int minElevation)
	{
		Name = name;
		ID = id;
		BlockHardness = blockHardness;
		Color = color;
		Range = range;
		MaxElevation = maxElevation;
		MinElevation = minElevation;
		null = false;
	}

	FBlock(const FName& name, const int& id, Hardness blockHardness)
	{
		Name = name;
		ID = id;
		BlockHardness = blockHardness;
	}
};