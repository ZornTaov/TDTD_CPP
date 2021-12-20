// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridWorldLayer.h"
#include "GridWorld.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class TDTD_CPP_API UGridWorld final : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Depth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UGridWorldLayer*> Layers;
public:
	int TileCount = 0;
	UGridWorld();
	void Init();
};
