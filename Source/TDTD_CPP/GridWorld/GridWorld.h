// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridWorldLayer.h"
#include "GridWorld.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, hidecategories=(AssetUserData,Activation,Collision,Cooking))
class TDTD_CPP_API UGridWorld final : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, NoClear, EditFixedSize)
	TArray<FGridWorldLayer> Layers;
public:
	UTile* GetTileAtWorldPos(FVector InPos);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Width = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Height = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Depth = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TileWidth = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TileThickness = 100;
	UFUNCTION(BlueprintCallable)
	void Init(ETileType InitType = ETileType::Empty);
	FVector Size() const {return FVector(Width, Height, Depth);}
	FVector TileSize() const {return FVector(TileWidth, TileWidth, TileThickness);}
	FGridWorldLayer* GetLayer(int Index);
	UTile* GetTileAt(FVector Pos);
	UTile* GetTileAt(int X, int Y, int Z);
	
	TArray<UTile*> GetNeighborTiles(const FVector IndexPos);
};
