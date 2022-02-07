// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileType.h"
#include "Engine/DataTable.h"
#include "GridWorld.h"
#include "Tile.generated.h"

class UJob;
DECLARE_LOG_CATEGORY_EXTERN(LogTile, Log, All);

class UInstalledObject;

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
UCLASS(BlueprintType, Blueprintable)
class TDTD_CPP_API UTile : public UObject
{
	GENERATED_BODY()
	UTile() = default;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType Type = ETileType::Empty;
	ETileType OldType = ETileType::Empty;
public:
	ETileType GetOldType() const;
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FTransform Pos = FTransform::Identity;
	FOnTileChangedDispatcher OnTileChangedCB;

	float MovementCost;
	
public:
	void Init(UGridWorld* Gw, int X, int Y, int Z, ETileType InitType = ETileType::Empty);
	float GetMovementCost() const;
	UPROPERTY(BlueprintReadOnly)
	UGridWorld* World = nullptr;
	UPROPERTY()
	UInstalledObject* InstalledObject = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TArray<UJob*> PendingJobs;
	
	UFUNCTION(BlueprintCallable)
	FTransform& GetPos();
	UFUNCTION(BlueprintCallable)
	ETileType GetType() const;
	void SetType(ETileType InType);
	UFUNCTION(BlueprintCallable)
	FVector GetWorldPos() const;
	UFUNCTION(BlueprintCallable)
	FVector GetIndexPos() const;
	void SetPos(const FVector& InPos);
	UFUNCTION(BlueprintCallable)
	FRotator GetRot() const;
	void SetRot(const FRotator& InRot);
	bool PlaceObject(UInstalledObject* Obj);
	void RegisterTileChanged(const FOnTileChanged& Del);
	UFUNCTION(BlueprintCallable)
	bool IsNeighbour(UTile* InTile, bool DiagOkay);
};

