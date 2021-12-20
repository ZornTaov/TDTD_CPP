// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.generated.h"


class UGridWorld;

UENUM(BlueprintType)
enum ETileType { Empty, Ground, Floor };

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct TDTD_CPP_API FTile
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETileType> Type = Empty;
public:
	TEnumAsByte<ETileType>& Type1()
	{
		return Type;
	}

	void SetType1(const TEnumAsByte<ETileType>& Type)
	{
		this->Type = Type;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGridWorld* World;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Pos;
public:
	FTile();
	void Init(UGridWorld* Gw);
	void SetType(const ETileType T);
	static FTile* Make(UGridWorld* Gw);
};

