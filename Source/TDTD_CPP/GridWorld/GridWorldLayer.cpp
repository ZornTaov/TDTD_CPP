// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldLayer.h"
#include "GridWorld.h"
#include "VarDump.h"

void FGridWorldLayer::Init(UGridWorld* Gw, const int Index, const ETileType InitType)
{
	World = Gw; 
	Depth = Index; 
	InnerWidth = Gw->Width; 
	InnerHeight = Gw->Height;
	Tiles.SetNum(InnerHeight);
	
	for (int Y = 0; Y < this->World->Height; ++Y)
	{
		FTile2DArray* TileRow = Tiles.IsValidIndex(Y) ? &Tiles[Y] : new FTile2DArray();
		for (int X = 0; X < this->World->Width; ++X)
		{
			if(!TileRow->IsValidIndex(X))
				TileRow->Add(FTile::Make(Gw, X, Y, Index, InitType));
		}
		// SetNum afterwards in case it needs to shrink
		TileRow->SetNum(InnerWidth);
		if (!Tiles.IsValidIndex(Y))
			Tiles.EmplaceAt(Y, *TileRow);
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
			FTile* Tile = Tiles[Y][X];
			return Tile;
		}
	}
	return nullptr;
}

int FGridWorldLayer::GetDepth() const
{
	return Depth;
}

bool FGridWorldLayer::IsWorldResized() const
{
	return World->Height != InnerHeight || World->Width != InnerWidth;
}
