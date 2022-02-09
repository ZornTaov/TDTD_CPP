// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorld.h"
#include "GridWorldLayer.h"
#include "Tile.h"
#include "VarDump.h"

void UGridWorld::RegisterFurnitureCreated(const FOnFurnitureCreated& Del)
{
	OnFurnitureCreatedCB.Add(Del);
}

void UGridWorld::RegisterTileChanged(const FOnTileChanged& Del)
{
	OnTileChangedCB.Add(Del);
}

UTile* UGridWorld::GetTileAtWorldPos(const FVector InPos)
{
	return GetTileAt(InPos/TileSize());
}

bool UGridWorld::IsTileWalkable(const UTile* InTile)
{
	return InTile->GetMovementCost() > 0 && InTile->PendingJobs.Num() == 0 && InTile->InstalledObject == nullptr;
}

bool UGridWorld::IsLocationWalkable(const FIntPoint& InPos)
{
	return IsTileWalkable(GetTileAt(FVector(InPos, 0)));
}

void UGridWorld::Init(const ETileType InitType)
{
	// TODO: Allow for resizing of Layers[] and Layers->Tiles[][]
	Layers.SetNum(Depth);
	for (int i = 0; i < Depth; ++i)
	{
		if(Layers[i].GetDepth() == -1 || Layers[i].GetDepth() != i || Layers[i].IsWorldResized())
			Layers[i].Init(this, i, InitType);
	}
}

FGridWorldLayer* UGridWorld::GetLayer(const int Index)
{
	if (Index >= 0 && Index < Depth)
	{
		return &Layers[Index];	
	}
	return nullptr;
}

UTile* UGridWorld::GetTileAt(const FVector Pos)
{
	return GetTileAt(Pos.X, Pos.Y, Pos.Z);
}

UTile* UGridWorld::GetTileAt(const int X, const int Y, const int Z)
{
	FGridWorldLayer* Layer = GetLayer(Z);
	if (!Layer)
	{
		UE_LOG(LogActor, Error, TEXT("No layers?! Tried: %d"), Z);
		return nullptr;
	}
	return Layer->GetTileAt(X, Y);
}

TArray<UTile*> UGridWorld::GetNeighborTiles(const FVector IndexPos, int Range)
{
	TArray<UTile*> NeighborTiles;
	for (int y = -Range; y <= Range; ++y)
	{
		for (int x = -Range; x <= Range; ++x)
		{
			NeighborTiles.Add(this->GetTileAt(FVector(x, y, 0) + IndexPos));
		}
	}
	return NeighborTiles;
}

void UGridWorld::OnTileChanged(UTile* Tile) const
{
	OnTileChangedCB.Broadcast(Tile);
}
