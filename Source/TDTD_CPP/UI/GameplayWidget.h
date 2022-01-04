// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IconButtonWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "GridWorld/Tile.h"
#include "GameplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class TDTD_CPP_API UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGameplayWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnTileClickedEvent_Event(ETileType Type, FName IOName);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UHorizontalBox* TileEditorBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* FloorTileDataTable = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* WallTileDataTable = nullptr;
	TSubclassOf<UIconButtonWidget> IconWidgetBPClass;
};
