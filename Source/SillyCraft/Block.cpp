// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"

Block::Block(const FName& name, const int& id, const Hardness& hardness, const FColor& color, const bool& surface) :
	Name(name), BlockHardness(hardness), Color(color), ID(id), SurfaceBlock(surface)
{
}

Block::Block(const FName& name, const int& id, const Hardness& hardness, const bool& surface) :
	Name(name), BlockHardness(hardness), ID(id), SurfaceBlock(surface)
{
}


Block::~Block()
{
	//delete Noise;
}