// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameplayWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "IconButtonWidget.h"
#include "Camera/TopDownCameraController.h"
#include "Components/HorizontalBoxSlot.h"
#include "Kismet/GameplayStatics.h"

UGameplayWidget::UGameplayWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UIconButtonWidget> IconWidgetBP(TEXT("/Game/Hud/IconWidget_BP"));
	if (IconWidgetBP.Succeeded())
	{
		IconWidgetBPClass = IconWidgetBP.Class;
	}
}

void UGameplayWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	TileEditorBox->ClearChildren();
	if (IsValid(FloorTileDataTable))
	{
		TArray<FTileStruct*> OutTileRowArray;
		FloorTileDataTable->GetAllRows<FTileStruct>(TEXT("GridWorldController#InitFloorComponents"), OutTileRowArray);
		TArray<FName> Names = FloorTileDataTable->GetRowNames();

		for (int i = 0; i < OutTileRowArray.Num(); ++i)
		{
			UIconButtonWidget* IconButtonWidget = CreateWidget<UIconButtonWidget>(
				this, IconWidgetBPClass, FName(Names[i])
			);
			IconButtonWidget->SetTexture(OutTileRowArray[i]->Icon);
			IconButtonWidget->OnClickedEvent.AddDynamic(this, &UGameplayWidget::OnTileClickedEvent_Event);
			IconButtonWidget->Type = OutTileRowArray[i]->Type;
			UHorizontalBoxSlot* HBoxSlot = TileEditorBox->AddChildToHorizontalBox(IconButtonWidget);
			HBoxSlot->Padding = FMargin(4);
			UE_LOG(LogTemp, Display, TEXT("%s"), *Names[i].ToString());
		}
	}
	UE_LOG(LogTemp, Display, TEXT("%d"), TileEditorBox->GetChildrenCount());
}

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGameplayWidget::OnTileClickedEvent_Event(ETileType Type, FName IOName)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	ATopDownCameraController* TopDownCameraController = Cast<ATopDownCameraController>(PlayerController);
	if (TopDownCameraController == nullptr)
	{
		return;
	}
	TopDownCameraController->CurrentTileType = Type;
	TopDownCameraController->CurrentInstalledObjectType = IOName;
}
