// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "GridWorld/Tile.h"
#include "IconButtonWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TDTD_CPP_API UIconButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UIconButtonWidget(const FObjectInitializer& ObjectInitializer);
	UFUNCTION()
	void OnButtonClicked();

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClickedEvent, ETileType, Type, FName, IOName);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, BlueprintAssignable, BlueprintCallable,
		meta=(DisplayName="On Clicked Event", Category="Default", MultiLine="true", OverrideNativeName="OnClickedEvent"
		))
	FOnClickedEvent OnClickedEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* IconButton = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* ChromaKeyedMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter=SetTexture)
	UTexture* Texture = nullptr;
	UFUNCTION(BlueprintCallable)
	void SetTexture(UTexture* NewTexture);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InstalledObjectName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor ChromaKeyColor = FColor::Magenta;
};
