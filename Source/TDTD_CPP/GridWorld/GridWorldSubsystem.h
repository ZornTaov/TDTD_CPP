// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridWorldController.h"
#include "GridWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TDTD_CPP_API UGridWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	AGridWorldController* Gwc;
	UPROPERTY()
	UGridWorld* Gw;
public:
	AGridWorldController* GetGridWorldController() const;
	void SetGridWorldController(AGridWorldController* const InGwc);
	UGridWorld* GetGridWorld() const;
	void SetGridWorld(UGridWorld* const InGw);
};
