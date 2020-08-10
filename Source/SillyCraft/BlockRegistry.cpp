// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockRegistry.h"

BlockRegistry::BlockRegistry() : AirID(0)
{
	std::shared_ptr<Block> air = std::make_shared<Block>("Air", AirID, Block::Empty);
	AddBlock(air);

	std::shared_ptr<Block> stone = std::make_shared<Block>("Stone", 1, Block::Hardness::Hard, FColor::Silver, 0, 0, 0);
	AddBlock(stone);
	BaseBlockID = stone->ID;

	std::shared_ptr<Block> dirt = std::make_shared<Block>("Dirt", 2, Block::Hardness::Medium, FColor::Orange, 100, 5000, 0);
	AddBlock(dirt);

	std::shared_ptr<Block> sand = std::make_shared<Block>("Sand", 3, Block::Hardness::Soft, FColor::Yellow, 0, 0, -5000);
	AddBlock(sand);

	std::shared_ptr<Block> grass = std::make_shared<Block>("Grass", 4, Block::Hardness::Medium, FColor::Green, 0, 5000, 0);
	AddBlock(grass);
}

BlockRegistry::~BlockRegistry()
{
}

std::shared_ptr<Block> BlockRegistry::GetBaseBlock() const
{
	return GetBlock(BaseBlockID);
}


std::shared_ptr<Block> BlockRegistry::GetBlock(const int& id) const
{
	return m_blocks[id];
}

void BlockRegistry::AddBlock(const std::shared_ptr<Block> block)
{
	m_blocks.Add(block->ID, block);
}

void BlockRegistry::GetIDs(TArray<int>& outIDs) const
{
	m_blocks.GetKeys(outIDs);
}
