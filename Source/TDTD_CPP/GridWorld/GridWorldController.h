// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TileManagerComponent.h"
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
	UTile* GetTileAtWorldPos(const FVector& Loc) const;
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
public:
	UTileManagerComponent* GetTileManager() const;
protected:
	UPROPERTY(BlueprintReadOnly, EditFixedSize, Instanced, NoClear)
	TArray<UWallTypeComponent*> WallComponents;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* WallTileDataTable = nullptr;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void DrawDebug(FVector Pos, FString Str) const;
	void DrawTileDebug() const;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	void InitWallInstance(UTile* Tile);
	void InitWallComponents(const USceneComponent* ParentComp);
	void InitInstance();
	void ClearAllInstances();
	void ClearWallInstances(TArray<UWallTypeComponent*>& Components);
};
