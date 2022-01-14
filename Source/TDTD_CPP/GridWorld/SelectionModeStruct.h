// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SelectionModeEnum.h"
#include "Engine/DataTable.h"
#include "SelectionModeStruct.generated.h"

/**
 * 
 */
USTRUCT()
struct TDTD_CPP_API FSelectionModeStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EGwSelectionMode Mode;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EGwSelectionMode NextMode;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture* UIIcon;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UUserWidget> CursorWidget;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FLinearColor CursorColor;
	
	
};
