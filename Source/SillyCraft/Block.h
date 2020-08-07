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
	FColor Color;
	int Range;
	const int ID;
	const bool SurfaceBlock;

	Block(const FName& name, const int& id, const Hardness& hardness, const FColor& color, const bool& surface);
	Block(const FName& name, const int& id, const Hardness& hardness, const bool& surface);

	~Block();
};
