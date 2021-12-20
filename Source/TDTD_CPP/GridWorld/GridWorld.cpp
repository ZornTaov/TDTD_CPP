// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorld.h"
#include "GridWorldLayer.h"
#include "VarDump.h"

UGridWorld::UGridWorld()
{
	
}

void UGridWorld::Init()
{
	for (int i = 0; i < Depth; ++i)
	{
		Layers.Add(UGridWorldLayer::Make(this, Width, Height));
	}
	VARDUMP(this->TileCount);
}
