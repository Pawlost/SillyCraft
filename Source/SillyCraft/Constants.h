// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <FastNoiseWrapper.h>

namespace Constants {
	//Noise
	const static int Seed = 1334;
	const static EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::PerlinFractal;
	const static float NoiseFrequency = 0.1f;
	const static int MaxElevation = 20;

	//Chunk
	const static int ChunkSize = 32;
	const static float ChunkScale = 100;
	const static int ChunkLenght = ChunkScale * ChunkSize;
	const static int ChunkSize2D = ChunkSize * ChunkSize;
	const static int ChunkSize3D = ChunkSize * ChunkSize * ChunkSize;

	//Picking
	const static float PickingMultiplier = 100.0f;
	const static float PickingSpeed = 1.0f;
	const static float HighlightTrasparency = 0.1f;

	//Zones
	const static int Distance = 1;
	const static int MeshZone = 2 * Distance + 1;
	const static int FillZone = MeshZone + 2;

	static int MakeIndex(const int& y, const int& z, const int& x)
	{
		return y + z * Constants::ChunkSize + x * Constants::ChunkSize2D;
	}

}