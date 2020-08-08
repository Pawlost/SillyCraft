// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Constants.h"

/**
 * 
 */


class SILLYCRAFT_API Block
{
public:
	static enum Hardness {
		Empty, Soft, Medium, Hard
	};

	const FName Name;
	const Hardness BlockHardness;
	const FColor Color;
	const int MinElevation;
	const int MaxElevation;
	const int ID;
	const int Range;

	Block(const FName& name, const int& id, const Hardness& hardness, const FColor& color, const int& range, const int& maxElevation, const int& minElevation);
	Block(const FName& name, const int& id, const Hardness& hardness);

	~Block();
};
