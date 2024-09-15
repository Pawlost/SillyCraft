// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkBase.h"

void UChunkBase::SetChunkGridData(const TSharedPtr<FChunkGridData> chunkGridData)
{
	ChunkGridData = chunkGridData;
}
