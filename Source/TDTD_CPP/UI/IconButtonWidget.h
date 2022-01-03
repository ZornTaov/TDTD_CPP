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
	explicit UIconButtonWidget(const FObjectInitializer& ObjectInitializer);
	
public:
	virtual void NativeConstruct() override;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* Icon = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* IconButton = nullptr;
	UPROPERTY(BlueprintReadWrite)
	UMaterial* ChromaKeyedMat;
	UPROPERTY(BlueprintReadWrite, BlueprintSetter=SetTexture)
	UTexture* Texture = nullptr;
	UFUNCTION(BlueprintCallable)
	void SetTexture(UTexture* NewTexture);
	UPROPERTY(BlueprintReadWrite)
	ETileType Type;
	UPROPERTY(BlueprintReadWrite)
	FName InstalledObjectName;
	UPROPERTY(BlueprintReadWrite)
	FColor ChromaKeyColor = FColor::Magenta;
};
