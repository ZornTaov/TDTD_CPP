// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "GridWorld.h"

void FTile::SetType(const ETileType T)
{
	Type = T;
}

FTile::FTile(): World(nullptr)
{
}

void FTile::Init(UGridWorld* Gw) {
	this->World = Gw;
	if(Gw)
		Gw->TileCount++;
}

FTile* FTile::Make(UGridWorld* Gw)
{
	FTile* t = new FTile();
	t->Init(Gw);
	return t;
}

