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
	TMap<int, std::shared_ptr<Block>> m_blocks;
public:
	void AddBlock(const std::shared_ptr<Block> block);
	std::shared_ptr<Block> GetBlock(const int& id) const;
	std::shared_ptr<Block> GetBaseBlock() const;
	int BaseBlockID;
	const int AirID;
	void GetIDs(TArray<int>& outIDs) const;
	BlockRegistry();
	~BlockRegistry();
};
