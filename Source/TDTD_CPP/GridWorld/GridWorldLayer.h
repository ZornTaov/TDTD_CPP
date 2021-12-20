// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridWorldLayer.generated.h"

class UGridWorld;
struct FTile;

USTRUCT(BlueprintType)
struct FTile2DArray{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FTile> Ar;
public:
	FTile& operator[] (const int32 i) {
		return Ar[i];
	}
 
	void Add(FTile* Tile) {
		Ar.Add(*Tile);
	}
};
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class TDTD_CPP_API UGridWorldLayer final : public UObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int Width;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int Height;
	UPROPERTY()
	UGridWorld* World;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FTile2DArray> Tiles; 
public:
	UGridWorldLayer();
	void Init(UGridWorld* Gw, int W, int H);
	static UGridWorldLayer* Make(UGridWorld* Gw, int W, int H);
};
