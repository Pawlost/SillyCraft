// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockRegistry.h"

BlockRegistry::BlockRegistry()
{
	Block* air = new Block("Air", 0, Block::Empty, false);
	Air = air;
	AddBlock(air);
	Block* stone = new Block("Stone", 1, Block::Hardness::Hard, FColor::Silver, 0, 123, false, 1.2f);
	SetBaseBlock(stone);
}

BlockRegistry::~BlockRegistry()
{
}

Block* BlockRegistry::GetBaseBlock() const 
{
	return m_baseBlock;
}


Block* BlockRegistry::GetBlock(const int& id) const
{
	return m_blocks[id];
}

void BlockRegistry::AddBlock(Block* block)
{
	m_blocks.Add(block->ID, block);
}

void BlockRegistry::SetBaseBlock(Block* baseblock)
{
	m_baseBlock = baseblock;
	if (!m_blocks.Contains(baseblock->ID))
	{
		m_blocks.Add(baseblock->ID, baseblock);
	}
}

void BlockRegistry::SetBaseBlock(const int& id)
{
	m_baseBlock = m_blocks[id];
}
