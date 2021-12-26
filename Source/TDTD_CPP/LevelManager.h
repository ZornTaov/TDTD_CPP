// Fill out your copyright notice in the Description page of Project Settings.
// Gotten from jchildren/LevelManager.cpp on github

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GridWorld/Tile.h"
#include "LevelManager.generated.h"

USTRUCT(BlueprintType)
struct FStructLevel
{
	GENERATED_USTRUCT_BODY()

	/*
	*/
	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadWrite, NoClear)
	TArray<int32> Floors;

	/*
	Wall values are broken up into two separate arrays
	corresponding to their rotation, such that each instance
	will be on the line equal to a constant integer * tile size

	XWalls and YWalls
	x = constant    y = constant

	Previously this was handled with % 2 and a single array,
	but became overly complicated to edit values manually

	Values of 0 imply that no corresponding instances should
	be created in the location, while and integer less than
	the number supported wall types in the object (WallTypes)
	will correspond to an instance of a Instanced Static Mesh
	at that location, which can be set in the editor

	These numbers are agnostic to the wall type, but should
	correspond to the type of wall the generator used intends
	to create (i.e. plain walls or those with doors and windows)
	*/
	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadWrite, NoClear)
	TArray<int32> XWalls;

	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadWrite, NoClear)
	TArray<int32> YWalls;

	void Resize(const int32 X, const int32 Y)
	{
		Floors.SetNumZeroed(X * Y);
		XWalls.SetNumZeroed(X * Y + Y);
		YWalls.SetNumZeroed(X * Y + X);
	}


	FStructLevel(const int32 X, const int32 Y)
	{
		Floors.SetNumZeroed(X * Y);
		XWalls.SetNumZeroed(X * Y + Y);
		YWalls.SetNumZeroed(X * Y + X);
	}
	FStructLevel()
	{
		Floors.SetNumUninitialized(0);
		XWalls.SetNumUninitialized(0);
		YWalls.SetNumUninitialized(0);
	}
	void Destroy()
	{
		Floors.Empty();
		XWalls.Empty();
		YWalls.Empty();
	}

};

UCLASS()
class TDTD_CPP_API ALevelManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelManager();
	
	// Begin asynchronous clean-up
	virtual void BeginDestroy() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	//virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void ClearAllComponents();

	UFUNCTION()
	void ClearAllInstances();

	UFUNCTION()
	void InitAllComponents();

	UFUNCTION()
	void InitInstances();

	UFUNCTION()
	void ResizeLevels();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Map Size")
	int32 MaxX = 10;

	UPROPERTY(EditAnywhere, Category = "Map Size")
	int32 MaxY = 10;

	UPROPERTY(EditAnywhere, Category = "Map Size")
	int32 MaxZ = 1;

	UPROPERTY(EditAnywhere, Category = "Instance Size")
	int32 TileSize;

	UPROPERTY(EditAnywhere, Category = "Instance Size")
	int32 LevelHeight;

	UPROPERTY(EditAnywhere, Category = "Instance Size")
	int32 FloorThickness;

	/*UPROPERTY(EditAnywhere, Category = "Mesh Types")
	int32 FloorTypes;

	UPROPERTY(EditAnywhere, Category = "Mesh Types")
	int32 WallTypes;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize, NoClear, Category = "Data")
	TArray<FStructLevel> Levels;

	UPROPERTY(EditAnywhere, Category = "Level Generation")
	int32 RandomSeed;

	/*
	Component structure is as follows:

	RootComponent
	FloorsRootComponent
	FloorComponent0
	FloorComponent1
	...
	FloorComponent[n]
	WallsRootComponent
	WallComponent0
	WallComponent1
	...
	WallComponent[n]
	*/

	UPROPERTY(Instanced, NoClear)
	USphereComponent* MeshRootComponent;

	UPROPERTY(Instanced, NoClear)
	USphereComponent* FloorsRootComponent;

	UPROPERTY(Instanced, NoClear)
	USphereComponent* WallsRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize, Instanced, NoClear)
	TArray<UInstancedStaticMeshComponent*> FloorComponents;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize, Instanced, NoClear)
	TArray<UInstancedStaticMeshComponent*> WallComponents;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* TileDataTable;

private:

	// Utility functions
	UFUNCTION()
		void ClearComponents(TArray<UInstancedStaticMeshComponent*> &Components);

	UFUNCTION()
		void UnregisterComponents(TArray<UInstancedStaticMeshComponent*> &Components);

	UFUNCTION()
		void ClearInstances(TArray<UInstancedStaticMeshComponent*> &Components);

	UFUNCTION()
		void InitComponents(TArray<UInstancedStaticMeshComponent*>& Components,
		                    const FString BaseComponentName, USphereComponent* ParentComp);

	UFUNCTION()
		void AddComponents(TArray<UInstancedStaticMeshComponent*> &Components,
			int32 Count, FString BaseComponentName, USphereComponent* ParentComp);

	UFUNCTION()
		void PopComponents(TArray<UInstancedStaticMeshComponent*> &Components, int32 Count);

	UFUNCTION()
		UInstancedStaticMeshComponent* AddInstancedStaticMeshComponent(FTileStruct Tile, const FString ComponentName, USphereComponent* ParentComp);

	UFUNCTION(BlueprintCallable)
	void UpdateLevel();


public:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& E) override;
#endif
};