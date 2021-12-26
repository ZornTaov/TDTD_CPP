// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "GridWorldLayer.generated.h"

class UGridWorld;

USTRUCT(BlueprintType)
struct FTile2DArray{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, EditFixedSize)
	TArray<FTile> Ar;
public:
	FTile& operator[] (const int32 I) {
		return Ar[I];
	}
 
	void Add(const FTile* Tile) {
		Ar.Add(*Tile);
	}
};
/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct TDTD_CPP_API FGridWorldLayer
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	UGridWorld* World = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, EditFixedSize)
	TArray<FTile2DArray> Tiles;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Depth = -1;
public:
	int GetDepth() const;
	FGridWorldLayer() = default;
	explicit FGridWorldLayer(UGridWorld* Gw, int Index);

	FTile* GetTileAt(int X, int Y);
	
};
