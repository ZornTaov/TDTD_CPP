// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallManagerComponent.generated.h"

class UWallTypeComponent;
class UTile;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TDTD_CPP_API UWallManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWallManagerComponent();
	UPROPERTY(BlueprintReadOnly, EditFixedSize, Instanced, NoClear)
	TArray<UWallTypeComponent*> WallComponents;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* WallTileDataTable = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void InitWallInstance(UTile* Tile);
	void InitWallComponents(const USceneComponent* ParentComp);
	void ClearWallInstances();
	void PlaceWall(UTile* TileAt, FName NameToCheck, bool Remove = false);
};
