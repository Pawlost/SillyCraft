// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <FastNoiseWrapper.h>

namespace Constants {
	//General
	const static int  MaxElevation = 50;
	const static int ChunkCount = 32;

	//Noise
	const static int Seed = 1334;
	const static EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::PerlinFractal;
	const static float NoiseFrequency = 0.1f;

	//Chunk
	const static int ChunkSize = 32;
	const static int ChunkScale = 100;
	const static int ChunkLenght = ChunkScale * ChunkSize;
	const static int ChunkSize2D = ChunkSize * ChunkSize;
	const static int ChunkSize3D = ChunkSize * ChunkSize * ChunkSize;
}