// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tile.h"
#include "TileManagerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TDTD_CPP_API UTileManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTileManagerComponent();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* FloorTileDataTable = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditFixedSize, Instanced, NoClear)
	TArray<UInstancedStaticMeshComponent*> FloorComponents;
	UFUNCTION()
	void OnTileChanged(const UTile* TileDataRef, ETileType NewType) const;

	void InitFloorComponents(const USceneComponent* ParentComp);
	void ClearTileInstances();
	void GetIndex(const UTile* TileData, uint8 OldTypeIndex, int& Index) const;
	void InitTileInstance(FTransform TileTransform, UTile* Tile);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
