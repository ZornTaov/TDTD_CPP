// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldLayer.h"
#include "GridWorld.h"


int FGridWorldLayer::GetDepth() const
{
	return Depth;
}

FGridWorldLayer::FGridWorldLayer(UGridWorld* Gw, const int Index): World(Gw), Depth(Index)
{
	for (int Y = 0; Y < this->World->Height; ++Y)
	{
		FTile2DArray TileRow;
		for (int X = 0; X < this->World->Width; ++X)
		{
			TileRow.Add(FTile::Make(Gw, X, Y, Index));
		}
		Tiles.Add(TileRow);
	}
}

FTile* FGridWorldLayer::GetTileAt(const int X, const int Y)
{
	if (Depth == -1)
	{
		UE_LOG(LogActor, Warning, TEXT("Uninitialized Layer."))
		return nullptr;
	}
	if (X >= 0 && X < World->Width)
	{
		if (Y >= 0 && Y < World->Height)
		{
			return &Tiles[X][Y];
		}
	}
	return nullptr;
}
