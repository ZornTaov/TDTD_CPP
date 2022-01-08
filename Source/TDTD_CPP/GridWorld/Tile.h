// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Tile.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTile, Log, All);


class UInstalledObject;
class UGridWorld;

UENUM(BlueprintType)
enum class ETileType : uint8 { Empty, Ground, Floor, Ramp };

template<typename T>
static FString GetEnumName(T InValue)
{
	return StaticEnum<T>()->GetNameStringByIndex(static_cast<int32>(InValue));
}

USTRUCT(BlueprintType)
struct FTileStruct : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ETileType Type = ETileType::Ground;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMesh* Mesh = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MovementSpeed = 1.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture* Icon = nullptr;
};

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct TDTD_CPP_API FTile
{
	GENERATED_BODY()
	FTile() = default;
	FTile(UGridWorld* Gw, int X, int Y, int Z);
	static FTile* Make(UGridWorld* Gw, int X, int Y, int Z, ETileType InitType = ETileType::Empty);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType Type = ETileType::Empty;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//EWallType WallType = EWallType::Empty;
	UPROPERTY(BlueprintReadOnly)
	UGridWorld* World = nullptr;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FTransform Pos = FTransform::Identity;

public:
	UPROPERTY()
	UInstalledObject* InstalledObject = nullptr;
	FTransform& GetPos();
	ETileType GetType() const;
	void SetType(ETileType InType);
	FVector GetWorldPos() const;
	FVector GetIndexPos() const;
	void SetPos(const FVector& InPos);
	FRotator GetRot() const;
	void SetRot(const FRotator& InRot);
	bool PlaceObject(UInstalledObject* Obj);
};

