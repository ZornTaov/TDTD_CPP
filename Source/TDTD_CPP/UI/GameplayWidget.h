// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IconButtonWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetSwitcher.h"
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
	UIconButtonWidget* AddIconButton(UPanelWidget* ParentWidget, FString Name, UTexture* NewTexture, ETileType Type = ETileType::Empty, FName IOName = "");
	void AddTileButtons(UHorizontalBox* HorizontalBox);
	void AddWallButtons(UHorizontalBox* HorizontalBox);
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnTileClickedEvent(ETileType Type, FName IOName);
	UFUNCTION()
	void OnWallClickedEvent(ETileType Type, FName IOName);
	UFUNCTION()
	void OnModeChangeClickedEvent(UIconButtonWidget* Widget);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UHorizontalBox* TileEditorBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UHorizontalBox* WallEditorBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UHorizontalBox* ActionHBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UPanelWidget* MenuHBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UWidgetSwitcher* ModeWidgetSwitcher;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* FloorTileDataTable = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDataTable* WallTileDataTable = nullptr;
	TSubclassOf<UIconButtonWidget> IconWidgetBPClass;
};
