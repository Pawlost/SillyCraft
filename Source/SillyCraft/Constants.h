// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <FastNoiseWrapper.h>

namespace Constants {
	//Chunk
	const static float ChunkScale = 100;
	const static int ChunkSize = 32;
	const static int ChunkSize2D = ChunkSize * ChunkSize;
	const static int ChunkSize3D = ChunkSize * ChunkSize * ChunkSize;
	const static int ChunkLenght = ChunkScale * ChunkSize;

	static int MakeIndex(const int& y, const int& z, const int& x)
	{
		return y + z * Constants::ChunkSize + x * Constants::ChunkSize2D;
	}
}