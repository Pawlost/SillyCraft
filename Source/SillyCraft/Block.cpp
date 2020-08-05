// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"

Block::Block(const FName& name, const int& id, const Hardness& hardness, const FColor& color, const int& range, const int& seed, const bool& surface, const float& frequency) :
	Name(name), BlockHardness(hardness), Color(color), Range(range), Noise(FastNoise(seed)), ID(id), SurfaceBlock(surface)
{
	Noise.SetFrequency(frequency);
}

Block::Block(const FName& name, const int& id, const Hardness& hardness, const bool& surface) :
	Name(name), BlockHardness(hardness), ID(id), SurfaceBlock(surface)
{
}

int Block::Get(const int& x, const int& z) const
{	
	return abs(Noise.GetPerlinFractal(x / Constants::Longitude, z / Constants::Latitude)) * Constants::MaxElevation;
}

Block::~Block()
{
	//delete Noise;
}
