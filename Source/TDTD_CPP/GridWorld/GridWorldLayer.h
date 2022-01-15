// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileType.h"
#include "VarDump.h"
#include "GridWorldLayer.generated.h"

class UGridWorld;
class UTile;
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
	UPROPERTY(BlueprintReadWrite, NoClear, EditFixedSize)
	TArray<UTile*> Tiles;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Depth = -1;
	int InnerWidth = 0;
	int InnerHeight = 0;
public:
	int GetDepth() const;
	bool IsWorldResized() const;
	FGridWorldLayer() = default;

	void Init(UGridWorld* Gw, int Index, ETileType InitType = ETileType::Empty);
	UTile* GetTileAt(int X, int Y);
	
};
