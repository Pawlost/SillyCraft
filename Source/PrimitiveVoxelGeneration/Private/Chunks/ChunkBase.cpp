﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunks/ChunkBase.h"

void UChunkBase::SetChunkSettings(const TSharedPtr<FChunkSettings> chunkSettings)
{
	this->ChunkSettings = chunkSettings;
}
