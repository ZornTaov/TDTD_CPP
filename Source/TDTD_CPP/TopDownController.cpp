// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownController.h"

#include "AIController.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Units/BaseUnitCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TDTDExtensionHelpers.h"
#include "VarDump.h"
#include "Blueprint/UserWidget.h"
#include "Camera/TDCameraControllerComponent.h"
#include "Engine/DecalActor.h"
#include "GridWorld/GridWorld.h"
#include "Installables/InstalledObject.h"
#include "GridWorld/SelectionDecalActor.h"
#include "GridWorld/SelectionModeEnum.h"
#include "UI/GameplayWidget.h"

ATopDownController::ATopDownController()
{
	bShowMouseCursor = true;
	//DefaultMouseCursor = EMouseCursor::Crosshairs;
	
	CameraController = CreateDefaultSubobject<UTDCameraControllerComponent>(TEXT("CameraController"));
	AddOwnedComponent(CameraController);
	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> DecalMaterialAsset(
		TEXT("MaterialInstanceConstant'/Game/Materials/M_GridDecal_Inst.M_GridDecal_Inst'"));
	if (DecalMaterialAsset.Succeeded())
	{
		ActionDecal = DecalMaterialAsset.Object;
	}
	
	this->MaterialParameterCollectionAsset = FSoftObjectPath(TEXT("/Game/Materials/CustomMouseCursorParams.CustomMouseCursorParams"));
	
	static ConstructorHelpers::FClassFinder<UGameplayWidget> WidgetClass(TEXT("/Game/Hud/GwEditorWidget"));
	if (WidgetClass.Succeeded())
	{
		this->GameplayWidgetClass = WidgetClass.Class;
	}
}

void ATopDownController::BeginPlay()
{
	Super::BeginPlay();
	SelectionDecal = GetWorld()->SpawnActor<ASelectionDecalActor>(DragStartPosition, FRotator());
	if (SelectionDecal)
	{
		SelectionDecal->SetDecalMaterial(ActionDecal);
	}
	GameplayWidget = CreateWidget<UGameplayWidget>(this, this->GameplayWidgetClass);
	GameplayWidget->AddToViewport();
	TArray<ABaseUnitCharacter*> PreSpawnedUnits;
	FindAllActors(GetWorld(), PreSpawnedUnits);
	for (ABaseUnitCharacter* PreSpawnedUnit : PreSpawnedUnits)
	{
		PreSpawnedUnit->GridWorldController = GetWorldController();
	}
	//FString MyActorName = GetActorLabel();
	//VARDUMP(SelectedUnits.Num(), VARDUMP(FName("SelectedUnits")));
}

AGridWorldController* ATopDownController::GetWorldController() const
{
	return WorldController;
}

void ATopDownController::SetWorldController(AGridWorldController* const InWorldController)
{
	this->WorldController = InWorldController;
}

float ATopDownController::GetTileSize() const
{
	return GetWorldController() &&
		GetWorldController()->GetGridWorld() ?
			GetWorldController()->GetGridWorld()->TileWidth : 200.0f;
}

float ATopDownController::GetTileThickness() const
{
	return GetWorldController() &&
		GetWorldController()->GetGridWorld() ?
			GetWorldController()->GetGridWorld()->TileThickness : 200.0f;
}

TArray<ABaseUnitCharacter*>& ATopDownController::GetSelectedUnits()
{
	return SelectedUnits;
}

void ATopDownController::DeselectUnits()
{
	if (SelectedUnits.Num() > 0)
	{
		for (const auto Unit : SelectedUnits)
		{
			Unit->SelectionCursor->SetVisibility(false);
		}
	}
	SelectedUnits.Empty();
}

void ATopDownController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (bIsDragging)
	{
		WhileDragging();
	}
}

void ATopDownController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("Interact", IE_Pressed, this,
		&ATopDownController::OnInteractPressed);
	InputComponent->BindAction("Interact", IE_Released, this,
	&ATopDownController::OnInteractReleased);
	InputComponent->BindAction("CancelOrExit", IE_Pressed, this,
		&ATopDownController::OnCancelOrExitPressed);
	InputComponent->BindAction("RotateTile", IE_Pressed, this,
		&ATopDownController::OnRotateTiePressed);

	if (CameraController)
	{
		InputComponent->BindAction("FocusCamera", IE_Pressed, CameraController,
			&UTDCameraControllerComponent::OnFocusCameraPressed);
		InputComponent->BindAction("ResetCameraRot", IE_Pressed, CameraController,
			&UTDCameraControllerComponent::OnResetCameraRotPressed);
		InputComponent->BindAction("ManipulateCamera", IE_Pressed, CameraController,
			&UTDCameraControllerComponent::OnManipulateCameraPressed);
		InputComponent->BindAction("ManipulateCamera", IE_Released, CameraController,
			&UTDCameraControllerComponent::OnManipulateCameraReleased);
		InputComponent->BindAction("ManipulateCameraRot", IE_Pressed, CameraController,
			&UTDCameraControllerComponent::OnManipulateCameraRotPressed);
		InputComponent->BindAction("ManipulateCameraRot", IE_Released, CameraController,
			&UTDCameraControllerComponent::OnManipulateCameraRotReleased);
		InputComponent->BindAxis("MoveForward", CameraController, &UTDCameraControllerComponent::MoveCameraY);
		InputComponent->BindAxis("MoveRight", CameraController, &UTDCameraControllerComponent::MoveCameraX);
		InputComponent->BindAxis("Pitch", CameraController, &UTDCameraControllerComponent::PitchCamera);
		InputComponent->BindAxis("TurnRate", CameraController, &UTDCameraControllerComponent::RotateCamera);
		InputComponent->BindAxis("Zoom", CameraController, &UTDCameraControllerComponent::ZoomCamera);
	}
	// support touch devices 
	//InputComponent->BindTouch(IE_Pressed, this, &ATopDownController::MoveToTouchLocation);
	//InputComponent->BindTouch(IE_Repeat, this, &ATopDownController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &ATopDownController::OnResetVR);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ATopDownController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATopDownController::OnWallInstallDone(UJob* Job, FName InstalledObjectType)
{
	Job->GetTile()->Jobs.Remove(Job);
	this->GetWorldController()->InstallWallToTile(Job->GetTile(), InstalledObjectType);
}

void ATopDownController::InteractUnderMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (const ATopDownCameraCharacter* CameraPawn = Cast<ATopDownCameraCharacter>(GetPawn()))
		{
			if (CameraPawn->GetCursorToWorld())
			{
				SetNewMoveDestination(CameraPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Camera, false, Hit);
		if (Hit.bBlockingHit)
		{
			/*if(GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow,
				FString::Printf(TEXT("Clicked %s"), *Hit.Actor->GetFName().ToString()));*/
			switch (CurrentMode) {
			case EGwSelectionMode::Building:
				//if (Hit.Actor->IsA(AGridWorldController::StaticClass()))
				{
					//WorldController = Cast<AGridWorldController>(Hit.Actor);
					for (FVector Loc : SelectedTilesLocations)
					{
						UTile* Tile = GetWorldController()->GetTileAtWorldPos(Loc);
						GetWorldController()->TileClicked(Tile,this->CurrentTileType);
					}
					SelectedTilesLocations.Empty();
				}
				break;
			case EGwSelectionMode::Installing:
				//if (Hit.Actor->IsA(AGridWorldController::StaticClass()))
				{				
					//WorldController = Cast<AGridWorldController>(Hit.Actor);
					for (FVector Loc : SelectedTilesLocations)
					{
						UTile* TileAt = GetWorldController()->GetTileAtWorldPos(Loc);
						const FName InstalledObjectType = this->CurrentInstalledObjectType;

						if (UInstalledObject::IsValidPosition(TileAt,InstalledObjectType.IsEqual(FName("Empty"))) && TileAt->Jobs.Num() == 0)
						{
							UJob* Job = GetWorldController()->GetJobSystem()->MakeJob(TileAt);
							TileAt->Jobs.Add(Job);
							
							Job->OnJobComplete.AddLambda([this, InstalledObjectType](UJob* InJob){OnWallInstallDone(InJob, InstalledObjectType);});
							Job->OnJobCancel.AddLambda([this](UJob* InJob){InJob->GetTile()->Jobs.Remove(InJob);});
						}
					}
					SelectedTilesLocations.Empty();
				}
				break;
			case EGwSelectionMode::Unit:				
				if (Hit.Actor->IsA(ABaseUnitCharacter::StaticClass()))
				{
					ABaseUnitCharacter* Unit = Cast<ABaseUnitCharacter>(Hit.Actor);
					if (SelectedUnits.Contains(Unit))
					{
						SelectedUnits.Remove(Unit);
						Unit->SelectionCursor->SetVisibility(false);
					}
					else
					{
						SelectedUnits.Add(Unit);
						Unit->SelectionCursor->SetVisibility(true);
					}
					//UPDATEDUMP(SelectedUnits.Num(), VARDUMP(FName("SelectedUnits")));
				}
				else if (SelectedUnits.Num() > 0)
				{
					FVector CursorL = Hit.ImpactPoint.GridSnap(GetTileSize());
					CursorL.Z = Hit.ImpactPoint.Z;
					// We hit something, move there
					SetNewMoveDestination(CursorL);
				}
				break;
			default:
				checkNoEntry();
			}
		}
	}
}

/*void ATopDownController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	const FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}*/

void ATopDownController::SetNewMoveDestination(const FVector DestLocation)
{
	int Counter = 0;
	const float Sides = FMath::RoundToFloat(FMath::Sqrt(static_cast<float>(GetSelectedUnits().Num())));
	for (ABaseUnitCharacter* Unit : GetSelectedUnits())
	{
		const int x = FMath::FloorToInt(Counter / Sides);
		const int y = FMath::Fmod(Counter, Sides);
		FVector Pos = DestLocation+FVector(x*100, y*100, 0);
		if (Unit)
		{
			float const Distance = FVector::Dist(Pos, Unit->GetActorLocation());

			AAIController* Controller = Unit->GetController<AAIController>();
			if(!ensure(Controller != nullptr)) return;
			// We need to issue move command only if far enough in order for walk animation to play correctly
			if ((Distance > 120.0f))
			{
				Controller->MoveToLocation(Pos, 5, true, true,
					false, false, nullptr, true);
				//UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, pos);
			}
		}
		Counter++;
	}
}


void ATopDownController::RotateTileUnderMouseCursor() const
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Camera, false, Hit);

	if (Hit.bBlockingHit)
	{
		if (CurrentMode == EGwSelectionMode::Building)
		{
			//if (Hit.Actor->IsA(AGridWorldController::StaticClass()))
			{
				//WorldController = Cast<AGridWorldController>(Hit.Actor);
				UTile* Tile = WorldController->GetTileAtWorldPos(Hit.Location.GridSnap(GetTileSize()));
				GetWorldController()->TileRotate(Tile);
			}
		}
	}
}

#pragma region CursorDrag
void ATopDownController::StartDrag()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Camera, false, Hit);

	if (Hit.bBlockingHit)
	{
		bIsDragging = true;
		bool Acted = false;
		switch (CurrentMode) {
		case EGwSelectionMode::Installing:
		case EGwSelectionMode::Building:
			//if (Hit.Actor->IsA(AGridWorldController::StaticClass()))
			{
				DragStartPosition = Hit.Location.GridSnap(GetTileSize());
				//Acted = true;
			}
			break;
		case EGwSelectionMode::Unit: 
			DragStartPosition = Hit.Location;
			Acted = true;
			break;
		default:
			checkNoEntry();
		}
		if (Acted)
		{
			if (SelectionDecal)
			{
				SelectionDecal->SetActorHiddenInGame(false);
			}
		}
	}
}

void ATopDownController::WhileDragging()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Camera, false, Hit);

	if (Hit.bBlockingHit)
	{
		switch (CurrentMode) {
		case EGwSelectionMode::Installing:
		case EGwSelectionMode::Building:
			//if (Hit.Actor->IsA(AGridWorldController::StaticClass()))
			{
				DragEndPosition = Hit.Location.GridSnap(GetTileSize());
			}
			break;
		case EGwSelectionMode::Unit: 
			DragEndPosition = Hit.Location;
			break;
		default: ;
		}
	}
	if (SelectionDecal)
	{
		int StartX = FMath::FloorToInt(DragStartPosition.X);
		int EndX = FMath::FloorToInt(DragEndPosition.X);
		if(EndX < StartX)
		{
			const int Tmp = EndX;
			EndX = StartX;
			StartX = Tmp;
		}

		int StartY = FMath::FloorToInt(DragStartPosition.Y);
		int EndY = FMath::FloorToInt(DragEndPosition.Y);
		if(EndY < StartY)
		{
			const int Tmp = EndY;
			EndY = StartY;
			StartY = Tmp;
		}
		const float ZSize = (StartX - EndX)*0.005f - (CurrentMode == EGwSelectionMode::Unit ? 0.0f : 1.05f);
		const float YSize = (StartY - EndY)*0.005f - (CurrentMode == EGwSelectionMode::Unit ? 0.0f : 1.05f);
		SelectionDecal->SetActorTransform(
			FTransform(
				FRotator(-90,0,0).Quaternion(),
				(DragStartPosition+DragEndPosition)/2,
				FVector(2, YSize, ZSize)
			));
		if (CurrentMode != EGwSelectionMode::Unit)
		{
			TArray<UTile* > SelectedTiles;
			for (int X = StartX; X <= EndX; X+=GetTileSize())
			{
				for (int Y = StartY; Y <= EndY; Y+=GetTileSize())
				{
					FVector Vector(X, Y, DragStartPosition.GridSnap(GetTileThickness()).Z);
					const FVector Pos = (Vector - GetWorldController()->GetActorLocation()) /
						GetWorldController()->GetGridWorld()->TileSize();
					if (CurrentMode == EGwSelectionMode::Installing)
					{
						if(X == StartX || X == EndX || Y == StartY || Y == EndY)
						{
							SelectedTiles.Add(GetWorldController()->GetGridWorld()->GetTileAt(Pos));
						}
					}
					else
					{
						SelectedTiles.Add(GetWorldController()->GetGridWorld()->GetTileAt(Pos));
					}
				}
			}
			SelectionDecal->OnSelectionChanged(SelectedTiles, [this](UTile* Tile)
			{
				if (!Tile)
					// no tile, ignore
					return FLinearColor::Black;
				
				{
					if (!Tile->InstalledObject)
						//new object
						return FLinearColor::White;

					if (this->CurrentInstalledObjectType == Tile->InstalledObject->ObjectType)
					{
						// don't color same
						return FLinearColor::Black;
					}
					// not same, must remove first
					return FLinearColor::Red;
				}
			});
		}
	}
}

bool ATopDownController::EndDrag()
{
	if (!bIsDragging)
	{
		return false;
	}
	bIsDragging = false;
	if (SelectionDecal)
	{
		SelectionDecal->SetActorHiddenInGame(true);
		TArray<UTile*> EmptyTiles;
		SelectionDecal->OnSelectionChanged(EmptyTiles, [this](UTile* Tile){return FLinearColor::Black;});
	}
	
	int StartX = FMath::FloorToInt(DragStartPosition.X);
	int EndX = FMath::FloorToInt(DragEndPosition.X);
	if(EndX < StartX)
	{
		const int Tmp = EndX;
		EndX = StartX;
		StartX = Tmp;
	}
	
	int StartY = FMath::FloorToInt(DragStartPosition.Y);
	int EndY = FMath::FloorToInt(DragEndPosition.Y);
	if(EndY < StartY)
	{
		const int Tmp = EndY;
		EndY = StartY;
		StartY = Tmp;
	}
	switch (CurrentMode)
	{
	case EGwSelectionMode::Building:
		{
			for (int X = StartX; X <= EndX; X+=GetTileSize())
			{
				for (int Y = StartY; Y <= EndY; Y+=GetTileSize())
				{
					SelectedTilesLocations.Add(FVector(X, Y, DragStartPosition.GridSnap(GetTileThickness()).Z));
				}
			}
	
			return false;
		}
	case EGwSelectionMode::Installing:
		{
			for (int X = StartX; X <= EndX; X+=GetTileSize())
			{
				for (int Y = StartY; Y <= EndY; Y+=GetTileSize())
				{
					if (X == StartX || X == EndX || Y == StartY || Y == EndY)
					{
						SelectedTilesLocations.Add(FVector(X, Y, DragStartPosition.GridSnap(GetTileThickness()).Z));
					}
				}
			}
	
			return false;
		}
	case EGwSelectionMode::Unit:
		{
			const int Current = SelectedUnits.Num();
			TArray<AActor*> OverlappedActors;
			SelectionDecal->GetOverlappingActors(OverlappedActors, ABaseUnitCharacter::StaticClass());
			for (AActor* OverlappedActor : OverlappedActors)
			{
				SelectedUnits.AddUnique(Cast<ABaseUnitCharacter>(OverlappedActor));
				Cast<ABaseUnitCharacter>(OverlappedActor)->SelectionCursor->SetVisibility(true);
			}
			return Current != SelectedUnits.Num();
		}
	default:
		checkNoEntry();
	}
	return false;
	/*if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow,
			FString::Printf(TEXT("Selected from %d,%d to %d,%d Count: %d"), StartX, StartY, EndX, EndY, SelectedTilesLocations.Num()));*/
}
#pragma endregion CursorDrag

#pragma region OnPressedReleased
void ATopDownController::OnInteractPressed()
{
	// set flag to keep updating destination until released
	BInteractUnderMouseCursor = true;
	StartDrag();
}

void ATopDownController::OnInteractReleased()
{
	// clear flag to indicate we should stop updating the destination
	BInteractUnderMouseCursor = false;
	if(!EndDrag())
		InteractUnderMouseCursor();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATopDownController::OnRotateTiePressed()
{
	RotateTileUnderMouseCursor();
}

void ATopDownController::OnCancelOrExitPressed()
{
	if (bIsDragging)
	{
		bIsDragging = false;
		if (SelectionDecal)
		{
			SelectionDecal->SetActorHiddenInGame(true);
		}
	}
}
#pragma endregion OnPressedReleased