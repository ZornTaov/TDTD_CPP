// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Installables/InstalledObject.h"

DEFINE_LOG_CATEGORY(LogTile);

void UTile::Init(UGridWorld* Gw, const int X, const int Y, const int Z, const ETileType InitType)
{
	this->Pos.SetLocation(FVector(X,Y,Z));
	this->World = Gw;
	this->SetType(InitType);
}

ETileType UTile::GetType() const
{
	return Type;
}

void UTile::SetType(const ETileType InType)
{
	OldType = this->Type;
	this->Type = InType;
	if (OnTileChangedCB.IsBound() && OldType != InType)
	{
		OnTileChangedCB.Broadcast(this);
	}
}

ETileType UTile::GetOldType() const
{
	return OldType;
}

FTransform& UTile::GetPos()
{
	return Pos;
}

FVector UTile::GetWorldPos() const
{
	if (World)
	{
		return this->GetIndexPos() * World->TileSize();
	}
	else
	{
		return FVector::ZeroVector;
	}
}

FVector UTile::GetIndexPos() const
{
	return Pos.GetLocation();
}

void UTile::SetPos(const FVector& InPos)
{
	this->Pos.SetLocation(InPos);
}

FRotator UTile::GetRot() const
{
	return Pos.GetRotation().Rotator();
}

void UTile::SetRot(const FRotator& InRot)
{
	this->Pos.SetRotation(InRot.Quaternion());
}

bool UTile::PlaceObject(UInstalledObject* Obj)
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

void UTile::RegisterTileChanged(const FOnTileChanged& Del)
{
	OnTileChangedCB.Add(Del);
}

bool UTile::IsNeighbour(UTile* InTile, bool DiagOkay)
{
	if (this->GetIndexPos().X == InTile->GetIndexPos().X &&
		FMath::Abs(this->GetIndexPos().Y - InTile->GetIndexPos().Y) == 1)
		return true;

	if (this->GetIndexPos().Y == InTile->GetIndexPos().Y &&
		FMath::Abs(this->GetIndexPos().X - InTile->GetIndexPos().X) == 1)
		return true;

	if (DiagOkay)
	{
		if (this->GetIndexPos().X == InTile->GetIndexPos().X-1 &&
			FMath::Abs(this->GetIndexPos().Y - InTile->GetIndexPos().Y) == 1)
				return true;
		
		if (this->GetIndexPos().X == InTile->GetIndexPos().X+1 &&
			FMath::Abs(this->GetIndexPos().Y - InTile->GetIndexPos().Y) == 1)
				return true;
	}
	
	return false;
}
