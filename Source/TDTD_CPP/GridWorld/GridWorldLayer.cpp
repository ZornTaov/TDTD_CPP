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
	Tiles.SetNum(InnerHeight * InnerWidth);
	
	for (int Y = 0; Y < this->World->Height; ++Y)
	{
		for (int X = 0; X < this->World->Width; ++X)
		{
			if(Tiles[X + Y * InnerWidth].World == nullptr)
				Tiles[X + Y * InnerWidth].Init(Gw, X, Y, Index, InitType);
		}
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
			FTile* Tile = &Tiles[X + Y * InnerWidth];
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
