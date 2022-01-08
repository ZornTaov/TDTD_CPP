// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "WallTypeComponent.generated.h"


class UGridWorld;
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TDTD_CPP_API UWallTypeComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UInstancedStaticMeshComponent* FillISM;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UInstancedStaticMeshComponent* InnerCornerISM;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UInstancedStaticMeshComponent* MiddleISM;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UInstancedStaticMeshComponent* OuterCornerISM;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, EditFixedSize, Instanced, NoClear)
	TArray<UInstancedStaticMeshComponent*> WallSubComponents;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName WallTypeName;
public:
	// Sets default values for this component's properties
	UWallTypeComponent();
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	void UpdateNeighborWalls(UGridWorld* World, const FVector& InstanceLocation) const;
	UFUNCTION(BlueprintCallable, Category="Components|WallTypeComponent")
	void AddWall(UGridWorld* World, const FVector& InstanceLocation, bool Propagate = true) const;
	UFUNCTION(BlueprintCallable, Category="Components|WallTypeComponent")
	void AddWalls(UGridWorld* World, const TArray<FVector>& InstanceLocations, bool Propagate = true) const;
	UFUNCTION(BlueprintCallable, Category = "Components|WallTypeComponent")
	void RemoveWall(UGridWorld* World, const FVector InstanceLocation, const bool Propagate = true) const;
	TArray<int> GetIndex(const UTile* TileData, uint8 OldTypeIndex) const;
	UFUNCTION(BlueprintCallable, Category="Components|WallTypeComponent")
	void ClearInstances();
	UFUNCTION(BlueprintCallable, Category = "Components|WallTypeComponent")
	int32 GetInstanceCount() const;
	//void PartialNavigationUpdate(int32 InstanceIdx);

};
