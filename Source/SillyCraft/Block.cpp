// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"

Block::Block(const FName& name, const int& id, const Hardness& hardness, const FColor& color, const int& range, const bool& surface, UFastNoiseWrapper* noise) :
	Name(name), BlockHardness(hardness), Color(color), Range(range), Noise(noise), ID(id), SurfaceBlock(surface)
{
}

Block::Block(const FName& name, const int& id, const Hardness& hardness, const bool& surface) :
	Name(name), BlockHardness(hardness), ID(id), SurfaceBlock(surface)
{
}

int Block::Get(const int& x, const int& z) const
{	
	double i = Noise->GetNoise2D(x, z);
	if (i > 0.0) {
		std::stringstream lol;
		lol << i;
		FString out(lol.str().c_str());
		UE_LOG(LogTemp, Display, TEXT("Elevation: %s"), *out);
	}
	return i;
}

Block::~Block()
{
	//delete Noise;
}