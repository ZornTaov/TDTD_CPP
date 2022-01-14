// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameplayWidget.h"

#include "TDTDExtensionHelpers.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "TopDownController.h"
#include "GridWorld/SelectionModeStruct.h"
#include "GridWorld/WallStruct.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

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
		IconButtonWidget->SetColorAndOpacity(FLinearColor(1,1,1));
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
	if (IsValid(SelectionModeDataTable))
	{
		TArray<FSelectionModeStruct*> OutModeRowArray;
		SelectionModeDataTable->GetAllRows<FSelectionModeStruct>(TEXT("UGameplayWidget#NativePreConstruct"), OutModeRowArray);
		FSelectionModeStruct* Row = OutModeRowArray[0];
		if (Row)
		{
			TileButtonWidget->SetTexture(Row->UIIcon);
			TileButtonWidget->Icon->SetColorAndOpacity(Row->CursorColor);
		}
	}
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
		FloorTileDataTable->GetAllRows<FTileStruct>(TEXT("UGameplayWidget#AddTileButtons"), OutTileRowArray);
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
		WallTileDataTable->GetAllRows<FWallStruct>(TEXT("UGameplayWidget#AddWallButtons"), OutWallRowArray);
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
	ATopDownController* TopDownCameraController = Cast<ATopDownController>(PlayerController);
	if (TopDownCameraController == nullptr)
	{
		return;
	}
	
	FLinearColor Color;
	EGwSelectionMode Mode = EGwSelectionMode::Building;
	UMaterialParameterCollectionInstance* Inst = GetWorld()->GetParameterCollectionInstance(
		CastChecked<UMaterialParameterCollection>(TopDownCameraController->MaterialParameterCollectionAsset.LoadSynchronous()));
	
	if (IsValid(SelectionModeDataTable))
	{
		FSelectionModeStruct* CurrRow = reinterpret_cast<FSelectionModeStruct*>(SelectionModeDataTable->FindRowUnchecked(FName(GetEnumName(TopDownCameraController->CurrentMode))));
		if (CurrRow)
		{
			Mode = CurrRow->NextMode;
			FSelectionModeStruct* Row = reinterpret_cast<FSelectionModeStruct*>(SelectionModeDataTable->FindRowUnchecked(FName(GetEnumName(Mode))));
			if (Row)
			{
				Widget->SetTexture(Row->UIIcon);
				Color = Row->CursorColor;
			}
		}
	}
	else
	{
		switch (TopDownCameraController->CurrentMode) {
		case EGwSelectionMode::Building:
			Mode = EGwSelectionMode::Installing;
			Color = FLinearColor(1,0,0,1);
			break;
		case EGwSelectionMode::Installing: 
			Mode = EGwSelectionMode::Unit;
			Color = FLinearColor(0,1,0,1);
			break;
		case EGwSelectionMode::Unit:
			Mode = EGwSelectionMode::Building;
			Color = FLinearColor(0,0.5f,1,1);
			break;
		default:
			checkNoEntry();
		}
	}
	TopDownCameraController->CurrentMode = Mode;
	Inst->SetVectorParameterValue(FName("Highlight Color"), Color);
	Widget->Icon->SetColorAndOpacity(Color);
	ModeWidgetSwitcher->SetActiveWidgetIndex(static_cast<uint8>(Mode));
			
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UGameplayWidget::OnTileClickedEvent(ETileType Type, FName IOName)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	ATopDownController* TopDownCameraController = Cast<ATopDownController>(PlayerController);
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
	ATopDownController* TopDownCameraController = Cast<ATopDownController>(PlayerController);
	if (TopDownCameraController == nullptr)
	{
		return;
	}
	TopDownCameraController->CurrentInstalledObjectType = IOName;
	UE_LOG(LogTemp, Display, TEXT("%s"), *IOName.ToString());
	
}
