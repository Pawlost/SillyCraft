// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"

Block::Block(const FName& name, const int& id, const Hardness& hardness) :
	Name(name), BlockHardness(hardness), ID(id), Range(0), Color(FColor::White), MaxElevation(0),MinElevation(0)
{
}

Block::Block(const FName& name, const int& id, const Hardness& hardness, const FColor& color, const int& range, const int& maxElevation, const int& minElevation) : Name(name), BlockHardness(hardness), Color(color), ID(id), Range(range), MaxElevation(maxElevation), MinElevation(minElevation)
{
}


Block::~Block()
{
	//delete Noise;
}