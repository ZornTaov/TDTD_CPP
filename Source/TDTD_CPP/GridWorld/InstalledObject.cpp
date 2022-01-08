// Fill out your copyright notice in the Description page of Project Settings.


#include "InstalledObject.h"

UInstalledObject* UInstalledObject::CreatePrototype(const FName Type, const float Cost, const int W, const int H)
{
	UInstalledObject* Obj = NewObject<UInstalledObject>();
	Obj->ObjectType = Type;
	Obj->MovementCost = Cost;
	Obj->Width = W;
	Obj->Height = H;
	return Obj;
}

UInstalledObject* UInstalledObject::PlaceInstance(const UInstalledObject* Proto, UTile* T)
{
	UInstalledObject* Obj = NewObject<UInstalledObject>();
	Obj->ObjectType = Proto->ObjectType;
	Obj->MovementCost = Proto->MovementCost;
	Obj->Width = Proto->Width;
	Obj->Height = Proto->Height;
	Obj->Tile = T;
	if(Obj->Tile->PlaceObject(Obj))
	{
		return nullptr;
	}
	return Obj;
}
