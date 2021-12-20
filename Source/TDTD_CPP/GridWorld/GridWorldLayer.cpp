// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldLayer.h"
#include "GridWorld.h"
#include "Tile.h"

UGridWorldLayer::UGridWorldLayer()
{
	
}

void UGridWorldLayer::Init(UGridWorld* Gw, const int W, const int H)
{
	this->Width = W;
	this->Height = H;
	this->World = Gw;
	for (int i = 0; i < this->Height; ++i)
	{
		FTile2DArray TileRow;
		for (int j = 0; j < this->Width; ++j)
		{
			TileRow.Add(FTile::Make(Gw));
		}
		Tiles.Add(TileRow);
	}
}


UGridWorldLayer* UGridWorldLayer::Make(UGridWorld* Gw, const int W, const int H)
{
	UGridWorldLayer* t = NewObject<UGridWorldLayer>(Gw);
	t->Init(Gw, W, H);
	return t;
}