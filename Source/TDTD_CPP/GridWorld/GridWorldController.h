// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
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
public:
	UGridWorld* GetGridWorld() const;
	void SetGridWorld(UGridWorld* const InWorld);
protected:
	UPROPERTY(Instanced, NoClear)
	USceneComponent* WorldRootComponent;
	
	UPROPERTY(Instanced, NoClear)
	USceneComponent* FloorsRootComponent;

	UPROPERTY(Instanced, NoClear)
	USceneComponent* WallsRootComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize, Instanced, NoClear)
	TArray<UInstancedStaticMeshComponent*> FloorComponents;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize, Instanced, NoClear)
	TArray<UInstancedStaticMeshComponent*> WallComponents;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* FloorTileDataTable = nullptr;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnTileTypeChanged(const FTile& TileDataRef, ETileType NewType) const;

	FTile* UpdateTile(FVector Pos, const ETileType& NewType, FTile* Tile = nullptr) const;
	FTile* UpdateTile(int X, int Y, int Z, ETileType NewType, FTile* InTile = nullptr) const;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	void InitComponents(TArray<UInstancedStaticMeshComponent*>& Components, const USceneComponent* ParentComp, const UDataTable* Data) const;
	void InitInstance();
	void ClearAllInstances();
	void ClearInstances(TArray<UInstancedStaticMeshComponent*> &Components);
	void TileClicked(const FVector& Vector) const;
	void TileRotate(const FVector& Vector) const;
};
