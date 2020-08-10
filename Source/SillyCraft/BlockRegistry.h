// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.h"

/**
 * 
 */
class SILLYCRAFT_API BlockRegistry
{
private:
	TMap<int, FBlock> m_blocks;
public:
	void AddBlock(FBlock block);
	FBlock GetBlock(const int& id) const;
	FBlock GetBaseBlock() const;
	int BaseBlockID;
	const int AirID;
	void GetIDs(TArray<int>& outIDs) const;
	BlockRegistry();
	~BlockRegistry();
};
