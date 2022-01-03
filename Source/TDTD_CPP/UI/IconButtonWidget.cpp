// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IconButtonWidget.h"

UIconButtonWidget::UIconButtonWidget(const FObjectInitializer& ObjectInitializer)
: UUserWidget(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UMaterial> ChromaKeyedMatFinder(TEXT("/Game/Thumbnails/Thumbnail_Chromakeyed_M"));
	if (ChromaKeyedMatFinder.Succeeded())
	{
		ChromaKeyedMat = ChromaKeyedMatFinder.Object; //UMaterialInstanceDynamic::Create(ChromaKeyedMatFinder.Object, this);
	}
	
}

void UIconButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(ChromaKeyedMat, this);
	if (Texture)
	{
		MaterialInstanceDynamic->SetTextureParameterValue(FName("Icon"),Texture);
		Icon->SetBrushFromMaterial(MaterialInstanceDynamic);
	}
}

void UIconButtonWidget::SetTexture(UTexture* NewTexture)
{
	UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(ChromaKeyedMat, this);
	if (NewTexture)
	{
		Texture = NewTexture;
	}
	if (Icon)
	{
		MaterialInstanceDynamic->SetTextureParameterValue(FName("Icon"),Texture);
		Icon->SetBrushFromMaterial(MaterialInstanceDynamic);
	}
}
