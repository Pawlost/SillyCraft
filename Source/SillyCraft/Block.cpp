// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"

Block::Block(const FName& name, const int& id, const Hardness& hardness, const FColor& color, const bool& surface) :
	Name(name), BlockHardness(hardness), Color(color), ID(id), SurfaceBlock(surface)
{
	InitNoise();
}

Block::Block(const FName& name, const int& id, const Hardness& hardness, const bool& surface) :
	Name(name), BlockHardness(hardness), ID(id), SurfaceBlock(surface)
{
	InitNoise();
}

void Block::InitNoise() {
	Noise = NewObject<UFastNoiseWrapper>();
	Noise->SetupFastNoise(Constants::NoiseType, Constants::Seed, Constants::NoiseFrequency);
}

int Block::Get(const int& x, const int& z) const
{	
	return abs(Noise->GetNoise2D(x, z)) * Constants::MaxElevation;
	/*if (i > 0.0) {
		std::stringstream lol;
		lol << i;
		FString out(lol.str().c_str());
		UE_LOG(LogTemp, Display, TEXT("Elevation: %s"), *out);
	}
	return i;*/
}

Block::~Block()
{
	//delete Noise;
}