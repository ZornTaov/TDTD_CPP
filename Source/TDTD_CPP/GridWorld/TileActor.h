// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileActor.generated.h"

UCLASS()
class TDTD_CPP_API ATileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileActor();
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Actor")
	USceneComponent* SceneRootComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Floor;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
