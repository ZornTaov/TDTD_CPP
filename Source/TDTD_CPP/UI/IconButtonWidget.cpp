// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IconButtonWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Camera/TopDownCameraController.h"

UIconButtonWidget::UIconButtonWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UMaterial> ChromaKeyedMatFinder(
		TEXT("/Game/Thumbnails/Thumbnail_Chromakeyed_M"));
	if (ChromaKeyedMatFinder.Succeeded())
	{
		ChromaKeyedMat = ChromaKeyedMatFinder.Object;
		//UMaterialInstanceDynamic::Create(ChromaKeyedMatFinder.Object, this);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UIconButtonWidget::OnButtonClicked()
{
	OnClickedEvent.Broadcast(Type, InstalledObjectName);
	OnModeClickedEvent.Broadcast(this);
	UE_LOG(LogTemp, Verbose, TEXT("Clicked: %s"), *GetFName().ToString());
	return;
	/*APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	ATopDownCameraController* TopDownCameraController = Cast<ATopDownCameraController>(PlayerController);
	if (TopDownCameraController == nullptr)
	{
		return;
	}
	if (Type != ETileType::Empty)
	{
		return;
	}
	if (IsValid(Icon))
	{
		FLinearColor Color(1, 1, 1, 1);
		switch (TopDownCameraController->CurrentMode)
		{
		case EGwSelectionMode::Building:
			Color = FLinearColor(0.0, 1.0, 0.0, 1.0);
			break;
		case EGwSelectionMode::Installing:
			Color = FLinearColor(0.0, 0.0, 1.0, 1.0);
			break;
		case EGwSelectionMode::Unit:
			Color = FLinearColor(1.0, 0.00, 0.0, 1.0);
			break;
		default:
			checkNoEntry();
		}
		Icon->SetColorAndOpacity(Color);
	}*/
}

void UIconButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UIconButtonWidget::NativeOnInitialized()
{
	if (IconButton)
	{
		IconButton->OnClicked.AddDynamic(this, &UIconButtonWidget::OnButtonClicked);
	}
	UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(ChromaKeyedMat, this);
	if (Texture)
	{
		MaterialInstanceDynamic->SetTextureParameterValue(FName("Icon"), Texture);
		Icon->SetBrushFromMaterial(MaterialInstanceDynamic);
	}

	Super::NativeOnInitialized();
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
		MaterialInstanceDynamic->SetTextureParameterValue(FName("Icon"), Texture);
		MaterialInstanceDynamic->SetVectorParameterValue(FName("ChromaKey"), ChromaKeyColor);
		Icon->SetBrushFromMaterial(MaterialInstanceDynamic);
	}
}
