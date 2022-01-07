// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridWorldLayer.h"
#include "GridWorld.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class TDTD_CPP_API UGridWorld final : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, EditFixedSize)
	TArray<FGridWorldLayer> Layers;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Width = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Height = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Depth = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TileSize = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TileThickness = 100;
	int TileCount = 0;
	UFUNCTION(BlueprintCallable)
	void Init(ETileType InitType = ETileType::Empty);
	FVector Size() const {return FVector(Width, Height, Depth);}
	FGridWorldLayer* GetLayer(int Index);
	FTile* GetTileAt(FVector Pos);
	FTile* GetTileAt(int X, int Y, int Z);
	
	TArray<FTile*> GetNeighborTiles(const FVector IndexPos);
};
