// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/VoxelGridGenerator.h"

UFastNoiseVoxelGenerator::UFastNoiseVoxelGenerator()
{
	Noise = CreateDefaultSubobject<UFastNoiseWrapper>("NoiseGenerator");
}