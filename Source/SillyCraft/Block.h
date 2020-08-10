// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

class SILLYCRAFT_API Block
{
private:
	int m_lifeSpan;

public:
	static enum Hardness {
		Empty, Soft, Medium, Hard
	};

	const FName Name;
	const Hardness BlockHardness;
	const int ID;
	const int Range;
	const FLinearColor Color;
	const int MinElevation;
	const int MaxElevation;
	

	Block(const FName& name, const int& id, const Hardness& hardness, const FLinearColor& color, const int& range, const int& maxElevation, const int& minElevation);
	Block(const FName& name, const int& id, const Hardness& hardness);

	void DecreaseLifeSpan(const int& decrease);
	void SetLifeSpan(const int& lifeSpan);
	int GetLifeSpan();
};
