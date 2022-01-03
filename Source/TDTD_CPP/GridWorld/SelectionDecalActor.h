// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "Components/BoxComponent.h"
#include "SelectionDecalActor.generated.h"

/**
 * 
 */
UCLASS()
class TDTD_CPP_API ASelectionDecalActor : public ADecalActor
{
	GENERATED_BODY()
	ASelectionDecalActor();

	UPROPERTY()
	UBoxComponent* Box;
};
