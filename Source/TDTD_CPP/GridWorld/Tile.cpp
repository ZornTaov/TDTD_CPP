// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "GridWorld.h"
#include "InstalledObject.h"

DEFINE_LOG_CATEGORY(LogTile);

void FTile::Init(UGridWorld* Gw, const int X, const int Y, const int Z, const ETileType InitType)
{
	this->Pos.SetLocation(FVector(X,Y,Z));
	this->World = Gw;
	this->SetType(InitType);
}

ETileType FTile::GetType() const
{
	return Type;
}

void FTile::SetType(const ETileType InType)
{
	this->Type = InType;
}

FTransform& FTile::GetPos()
{
	return Pos;
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

bool FTile::PlaceObject(UInstalledObject* Obj)
{
	if (Obj == nullptr)
	{
		//uninstall current object
		InstalledObject = nullptr;
		return true;
	}
	if (InstalledObject != nullptr)
	{
		//already has installed object
		UE_LOG(LogTile, Warning, TEXT("Trying to assign an installed object to a tile that has one!"));
		return false;
	}
	// install new object
	InstalledObject = Obj;
	return true;
}
