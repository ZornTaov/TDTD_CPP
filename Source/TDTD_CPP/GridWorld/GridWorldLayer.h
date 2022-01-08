// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "VarDump.h"
#include "GridWorldLayer.generated.h"

class UGridWorld;

USTRUCT(BlueprintType)
struct FTile2DArray{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, EditFixedSize)
	TArray<FTile> Ar;
public:
	FTile* operator[] (const int32 I) {
		FTile* Tile = &Ar[I];
		return Tile;
	}
 
	void Add(const FTile* Tile) {
		Ar.Add(*Tile);
	}

	void SetNum(TArray<FTile>::SizeType NewNum, bool bAllowShrinking = true)
	{
		Ar.SetNum(NewNum, bAllowShrinking);
	}

	bool IsValidIndex(TArray<FTile>::SizeType Index)
	{
		return Ar.IsValidIndex(Index);
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
	int InnerWidth = 0;
	int InnerHeight = 0;
public:
	int GetDepth() const;
	bool IsWorldResized() const;
	FGridWorldLayer() = default;

	void Init(UGridWorld* Gw, int Index, ETileType InitType = ETileType::Empty);
	FTile* GetTileAt(int X, int Y);
	
};
