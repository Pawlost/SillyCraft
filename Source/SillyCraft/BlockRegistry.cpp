// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockRegistry.h"

BlockRegistry::BlockRegistry() : AirID(0)
{
	FBlock air = FBlock("Air", AirID, FBlock::Empty);
	AddBlock(air);

	FBlock stone = FBlock("Stone", 1, FBlock::Hardness::Hard, FColor::Silver, 0, 0, 0);
	AddBlock(stone);
	BaseBlockID = stone.ID;

	FBlock dirt = FBlock("Dirt", 2, FBlock::Hardness::Medium, FColor::Orange, 100, 5000, 0);
	AddBlock(dirt);

	FBlock sand = FBlock("Sand", 3, FBlock::Hardness::Soft, FColor::Yellow, 0, 0, -5000);
	AddBlock(sand);

	FBlock grass = FBlock("Grass", 4, FBlock::Hardness::Medium, FColor::Green, 0, 5000, 0);
	AddBlock(grass);
}

BlockRegistry::~BlockRegistry()
{
}

FBlock BlockRegistry::GetBaseBlock() const
{
	return GetBlock(BaseBlockID);
}


FBlock BlockRegistry::GetBlock(const int& id) const
{
	return m_blocks[id];
}

void BlockRegistry::AddBlock(FBlock block)
{
	m_blocks.Add(block.ID, block);
}

void BlockRegistry::GetIDs(TArray<int>& outIDs) const
{
	m_blocks.GetKeys(outIDs);
}
