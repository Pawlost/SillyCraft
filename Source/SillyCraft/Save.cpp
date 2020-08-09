// Fill out your copyright notice in the Description page of Project Settings.


#include "Save.h"

void USave::Init()
{
	for (FPrimitiveChunk chunk : SavedChunks)
	{
		ChunkMap.Add(TTuple<int, int, int>(chunk.X, chunk.Y, chunk.Z), chunk);
	}
}

void USave::Deinit()
{
	SavedChunks.Empty();
	for (TPair<TTuple<int, int, int>, FPrimitiveChunk> chunk : ChunkMap)
	{
		SavedChunks.Add(chunk.Value);
	}
	ChunkMap.Empty();
}
