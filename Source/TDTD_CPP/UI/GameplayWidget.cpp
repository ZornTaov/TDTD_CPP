// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameplayWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Camera/TopDownCameraController.h"
#include "GridWorld/WallStruct.h"
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

UIconButtonWidget* UGameplayWidget::AddIconButton(UPanelWidget* ParentWidget, FString Name, UTexture* NewTexture, ETileType Type, FName IOName)
{
	UIconButtonWidget* IconButtonWidget = CreateWidget<UIconButtonWidget>(
		this, IconWidgetBPClass, FName(Name)
	);
	if (Name.Contains("Empty"))
	{
		IconButtonWidget->ChromaKeyColor = FLinearColor(1,1,1);
	}
	IconButtonWidget->SetTexture(NewTexture);
	IconButtonWidget->Type = Type;
	IconButtonWidget->InstalledObjectName = IOName;
	ParentWidget->AddChild(IconButtonWidget);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Name);
	return IconButtonWidget;
}

void UGameplayWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	TileEditorBox->ClearChildren();
	
	UIconButtonWidget* TileButtonWidget = AddIconButton(MenuHBox, FString("ModeSelect"), nullptr);
	TileButtonWidget->OnModeClickedEvent.AddDynamic(this, &UGameplayWidget::OnModeChangeClickedEvent);
	AddTileButtons(TileEditorBox);
	AddWallButtons(WallEditorBox);
	//AddActionButtons(ActionHBox);
}

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGameplayWidget::AddTileButtons(UHorizontalBox* HorizontalBox)
{
	if (IsValid(FloorTileDataTable))
	{
		TArray<FTileStruct*> OutTileRowArray;
		FloorTileDataTable->GetAllRows<FTileStruct>(TEXT("GridWorldController#InitFloorComponents"), OutTileRowArray);
		TArray<FName> Names = FloorTileDataTable->GetRowNames();
		for (int i = 0; i < OutTileRowArray.Num(); ++i)
		{
			UIconButtonWidget* TileButtonWidget = AddIconButton(HorizontalBox, FString("Tile") + Names[i].ToString(),
			                                                    OutTileRowArray[i]->Icon, OutTileRowArray[i]->Type);
			TileButtonWidget->OnClickedEvent.AddDynamic(this, &UGameplayWidget::OnTileClickedEvent);
		}
	}
}

void UGameplayWidget::AddWallButtons(UHorizontalBox* HorizontalBox)
{
	if (IsValid(WallTileDataTable))
	{
		TArray<FWallStruct*> OutWallRowArray;
		WallTileDataTable->GetAllRows<FWallStruct>(TEXT("GridWorldController#InitFloorComponents"), OutWallRowArray);
		TArray<FName> Names = WallTileDataTable->GetRowNames();
		for (int i = 0; i < OutWallRowArray.Num(); ++i)
		{
			UIconButtonWidget* TileButtonWidget = AddIconButton(HorizontalBox, FString("Wall") + Names[i].ToString(),
			                                                    OutWallRowArray[i]->Icon, ETileType::Empty, Names[i]);
			TileButtonWidget->OnClickedEvent.AddDynamic(this, &UGameplayWidget::OnWallClickedEvent);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UGameplayWidget::OnModeChangeClickedEvent(UIconButtonWidget* Widget)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	ATopDownCameraController* TopDownCameraController = Cast<ATopDownCameraController>(PlayerController);
	if (TopDownCameraController == nullptr)
	{
		return;
	}
	switch (TopDownCameraController->CurrentMode) {
	case EGwSelectionMode::Building:
		TopDownCameraController->CurrentMode = EGwSelectionMode::Installing;
		Widget->Icon->SetColorAndOpacity(FLinearColor(1,0,0,1));
		ModeWidgetSwitcher->SetActiveWidgetIndex(static_cast<uint8>(EGwSelectionMode::Installing));
		break;
	case EGwSelectionMode::Installing: 
		TopDownCameraController->CurrentMode = EGwSelectionMode::Unit;
		Widget->Icon->SetColorAndOpacity(FLinearColor(0,1,0,1));
		ModeWidgetSwitcher->SetActiveWidgetIndex(static_cast<uint8>(EGwSelectionMode::Unit));
		break;
	case EGwSelectionMode::Unit:
		TopDownCameraController->CurrentMode = EGwSelectionMode::Building;
		Widget->Icon->SetColorAndOpacity(FLinearColor(1,1,1,1));
		ModeWidgetSwitcher->SetActiveWidgetIndex(static_cast<uint8>(EGwSelectionMode::Building));
		break;
	default:
		checkNoEntry();
	}	
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGameplayWidget::OnTileClickedEvent(ETileType Type, FName IOName)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	ATopDownCameraController* TopDownCameraController = Cast<ATopDownCameraController>(PlayerController);
	if (TopDownCameraController == nullptr)
	{
		return;
	}
	TopDownCameraController->CurrentTileType = Type;
	if (Type == ETileType::Empty)
	{
		TopDownCameraController->CurrentInstalledObjectType = "";
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGameplayWidget::OnWallClickedEvent(ETileType Type, FName IOName)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	ATopDownCameraController* TopDownCameraController = Cast<ATopDownCameraController>(PlayerController);
	if (TopDownCameraController == nullptr)
	{
		return;
	}
	TopDownCameraController->CurrentInstalledObjectType = IOName;
	UE_LOG(LogTemp, Display, TEXT("%s"), *IOName.ToString());
	
}
