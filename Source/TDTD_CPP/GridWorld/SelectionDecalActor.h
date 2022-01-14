// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "SelectionDecalActor.generated.h"

class UTile;
/**
 * 
 */
UCLASS()
class TDTD_CPP_API ASelectionDecalActor : public ADecalActor
{
	GENERATED_BODY()
	ASelectionDecalActor();
	UPROPERTY()
	UCanvasRenderTarget2D* SelectionRenderTarget;
public:
	//void DrawToCanvasRenderTarget(UCanvas* Canvas, int32 Width, int32 Height);
	//UFUNCTION()
	void OnSelectionChanged(TArray<UTile*>& Tiles, TFunction<FLinearColor(UTile*)>Pred);
	UPROPERTY()
	UBoxComponent* Box;
};
