// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockRegistry.h"

BlockRegistry::BlockRegistry() : AirID(0)
{
	Block* air = new Block("Air", AirID, Block::Empty);
	AddBlock(air);

	Block* stone = new Block("Stone", 1, Block::Hardness::Hard, FColor::Silver, 0, 0, 0);
	AddBlock(stone);
	BaseBlockID = stone->ID;

	Block* dirt = new Block("Dirt", 2, Block::Hardness::Medium, FColor::Orange, 100, 5000, 0);
	AddBlock(dirt);

	Block* sand = new Block("Sand", 3, Block::Hardness::Soft, FColor::Yellow, 0, 0, -5000);
	AddBlock(sand);

	Block* grass = new Block("Grass", 4, Block::Hardness::Medium, FColor::Green, 0, 5000, 0);
	AddBlock(grass);
}

BlockRegistry::~BlockRegistry()
{
}

Block* BlockRegistry::GetBaseBlock() const 
{
	return GetBlock(BaseBlockID);
}


Block* BlockRegistry::GetBlock(const int& id) const
{
	return m_blocks[id];
}

void BlockRegistry::AddBlock(Block* block)
{
	m_blocks.Add(block->ID, block);
}

void BlockRegistry::GetIDs(TArray<int>& outIDs) const
{
	m_blocks.GetKeys(outIDs);
}
