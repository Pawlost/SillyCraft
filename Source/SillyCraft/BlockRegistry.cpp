// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockRegistry.h"

BlockRegistry::BlockRegistry() : AirID(0)
{
	Block* air = new Block("Air", AirID, Block::Empty, false);
	AddBlock(air);

	Block* stone = new Block("Stone", 1, Block::Hardness::Hard, FColor::Silver, false);
	AddBlock(stone);
	BaseBlockID = stone->ID;
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
