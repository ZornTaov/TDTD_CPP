// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileType.h"
#include "Installables/InstalledObject.h"
#include "JobSystem/JobSystem.h"
#include "GridWorldController.generated.h"

class UGridWorld;
class UTileManagerComponent;
class UWallManagerComponent;
UCLASS()
class TDTD_CPP_API AGridWorldController : public AActor 
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AGridWorldController();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGridWorld* World = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UJobSystem* JobSystem;
	UPROPERTY()
	TMap<FName, UInstalledObject*> Installables;
public:
	UJobSystem* GetJobSystem() const;
	UGridWorld* GetGridWorld() const;
	void SetGridWorld(UGridWorld* const InWorld);
	void InstallWallToTile(UTile* TileAt, FName InstalledObjectName, bool Remove = false);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTile* GetTileAtWorldPos(const FVector& Loc) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UTile*> GetNeighborTiles(const FVector IndexPos, int Range = 1) const;
	void PlaceGhost(UTile* InTile, const FName& InstalledObjectName);
	FVector GetWorldPosForTileCenter(const FIntPoint& InPoint) const;
	UTile* GetTileAtWorldPos(FVector InPos, FIntPoint& OutPoint);
	float GetTileSize() const;
	float GetTileThickness() const;
protected:
	UPROPERTY(Instanced, NoClear)
	USceneComponent* WorldRootComponent;
	
	UPROPERTY(Instanced, NoClear)
	USceneComponent* FloorsRootComponent;

	UPROPERTY(Instanced, NoClear)
	USceneComponent* WallsRootComponent;
	
	UPROPERTY(Instanced, NoClear)
	USceneComponent* InstalledObjectComponent;
	
	UPROPERTY(Instanced, NoClear)
	UTileManagerComponent* TileManager;
	UPROPERTY(Instanced, NoClear)
	UWallManagerComponent* WallManager;
public:
	UTileManagerComponent* GetTileManager() const;
	UWallManagerComponent* GetWallManager() const;
protected:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void OnInstalledObjectLoaded();

	virtual void PreInitializeComponents() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	void InitInstances() const;
	void ClearAllInstances() const;

	void TileClicked(UTile* Tile, ETileType NewType) const;
	void TileRotate(UTile* Tile) const;
	UTile* UpdateTile(ETileType NewType, UTile* InTile = nullptr) const;
};
