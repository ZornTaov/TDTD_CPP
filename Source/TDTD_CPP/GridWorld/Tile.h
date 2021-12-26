// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Tile.generated.h"


class UGridWorld;

UENUM(BlueprintType)
enum class ETileType : uint8 { Empty, Ground, Floor };

USTRUCT(BlueprintType)
struct FTileStruct : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ETileType Type = ETileType::Empty;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMesh* Mesh = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MovementSpeed = 1.0f;
};

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct TDTD_CPP_API FTile
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType Type = ETileType::Empty;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//EWallType WallType = EWallType::Empty;
	UPROPERTY(BlueprintReadOnly)
	UGridWorld* World = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform Pos = FTransform::Identity;
	
public:
	int InstanceIndex = -1;
	FTile() = default;
	FTile(UGridWorld* Gw, int X, int Y, int Z);
	static FTile* Make(UGridWorld* Gw, int X, int Y, int Z);
	ETileType GetType() const;
	void SetType(const ETileType& InType);
	FVector GetWorldPos() const;
	FVector GetIndexPos() const;
	void SetPos(const FVector& InPos);
	FRotator GetRot() const;
	void SetRot(const FRotator& InRot);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateTile, FTile*, Tile, ETileType, InType);
	FUpdateTile Del;
};

