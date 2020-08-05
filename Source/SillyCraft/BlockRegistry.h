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
	Block* m_baseBlock;
public:
	void AddBlock(Block* block);
	void SetBaseBlock(Block* block);
	void SetBaseBlock(const int& id);
	Block* GetBlock(const int& id) const;
	Block* GetBaseBlock() const;
	const Block* Air;
	BlockRegistry();
	~BlockRegistry();
};
