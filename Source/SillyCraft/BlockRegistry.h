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
	TMap<int, Block*> m_blocks;
public:
	void AddBlock(Block* block);
	Block* GetBlock(const int& id) const;
	Block* GetBaseBlock() const;
	int BaseBlockID;
	const int AirID;
	void GetIDs(TArray<int>& outIDs) const;
	BlockRegistry();
	~BlockRegistry();
};
