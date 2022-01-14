// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "WallTypeComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "JobSystem/JobSystem.h"
#include "GridWorldController.generated.h"

class ATileActor;
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
public:
	UJobSystem* GetJobSystem() const;
	UGridWorld* GetGridWorld() const;
	void SetGridWorld(UGridWorld* const InWorld);
	void InstallWallToTile(UTile* TileAt, FName InstalledObjectName, bool Remove = false);
protected:
	UPROPERTY(Instanced, NoClear)
	USceneComponent* WorldRootComponent;
	
	UPROPERTY(Instanced, NoClear)
	USceneComponent* FloorsRootComponent;

	UPROPERTY(Instanced, NoClear)
	USceneComponent* WallsRootComponent;
	
	UPROPERTY(Instanced, NoClear)
	USceneComponent* InstalledObjectComponent;
	
	UPROPERTY(BlueprintReadOnly, EditFixedSize, Instanced, NoClear)
	TArray<UInstancedStaticMeshComponent*> FloorComponents;

	UPROPERTY(BlueprintReadOnly, EditFixedSize, Instanced, NoClear)
	TArray<UWallTypeComponent*> WallComponents;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* FloorTileDataTable = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* WallTileDataTable = nullptr;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnTileChanged(const UTile* TileDataRef, ETileType NewType) const;

	UTile* UpdateTile(FVector Pos, const ETileType& NewType, UTile* Tile = nullptr) const;
	UTile* UpdateTile(int X, int Y, int Z, ETileType NewType, UTile* InTile = nullptr) const;
	void DrawTileDebug() const;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	void InitFloorComponents(TArray<UInstancedStaticMeshComponent*>& Components, const USceneComponent* ParentComp, const UDataTable* Data) const;
	void InitWallComponents(TArray<UWallTypeComponent*>& Components, const USceneComponent* ParentComp, const UDataTable* Data) const;
	void InitInstance();
	void ClearWallInstances(TArray<UWallTypeComponent*> Array);
	void ClearAllInstances();
	void ClearTileInstances(TArray<UInstancedStaticMeshComponent*>& Components);
	void TileClicked(const FVector& Vector, ETileType NewType) const;
	void TileRotate(const FVector& Vector) const;
	void GetIndex(const UTile* TileData, uint8 OldTypeIndex, int& Index) const;

	void DrawDebug(FVector Pos, FString Str) const;
};
