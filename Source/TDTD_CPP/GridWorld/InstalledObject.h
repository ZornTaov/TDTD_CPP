// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "InstalledObject.generated.h"

/**
 * 
 */
UCLASS()
class TDTD_CPP_API UInstalledObject : public UObject
{
	GENERATED_BODY()
	UTile* Tile;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	FName ObjectType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	float MovementCost;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	int Width;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	int Height;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	bool LinksToNeighbor;

	/*virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("Installed Object"), GetFName());
	}*/

	UInstalledObject() {  }
	
	static UInstalledObject* CreatePrototype(const FName Type, const float Cost = 1.0f, const int W = 1, const int H = 1);

	static UInstalledObject* PlaceInstance(const UInstalledObject* Proto, UTile* T);
};
