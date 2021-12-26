// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "GridWorld.h"

ETileType FTile::GetType() const
{
	return Type;
}

void FTile::SetType(const ETileType& InType)
{
	this->Type = InType;
	Del.Broadcast(this, InType);
}

FVector FTile::GetWorldPos() const
{
	if (World)
	{
		return this->GetIndexPos() * FVector(World->TileSize, World->TileSize, World->TileThickness);
	}
	else
	{
		return FVector::ZeroVector;
	}
}

FVector FTile::GetIndexPos() const
{
	return Pos.GetLocation();
}

void FTile::SetPos(const FVector& InPos)
{
	this->Pos.SetLocation(InPos);
}

FRotator FTile::GetRot() const
{
	return Pos.GetRotation().Rotator();
}

void FTile::SetRot(const FRotator& InRot)
{
	this->Pos.SetRotation(InRot.Quaternion());
}

FTile::FTile(UGridWorld* Gw, const int X, const int Y, const int Z)
{
	this->Pos.SetLocation(FVector(X,Y,Z));
	this->World = Gw;
	if(Gw)
		Gw->TileCount++;
}

FTile* FTile::Make(UGridWorld* Gw, const int X, const int Y, const int Z)
{
	FTile* t = new FTile(Gw, X, Y, Z);
	return t;
}

