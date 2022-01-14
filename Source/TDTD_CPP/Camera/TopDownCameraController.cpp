// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownCameraController.h"

#include "AIController.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "TopDownCameraCharacter.h"
#include "Units/BaseUnitCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "VarDump.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DecalActor.h"
#include "GameFramework/SpringArmComponent.h"
#include "GridWorld/GridWorld.h"
#include "GridWorld/SelectionDecalActor.h"
#include "GridWorld/SelectionModeEnum.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/GameplayWidget.h"


ATopDownCameraController::ATopDownCameraController()
{
	bShowMouseCursor = true;
	//DefaultMouseCursor = EMouseCursor::Crosshairs;
	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> DecalMaterialAsset(
		TEXT("MaterialInstanceConstant'/Game/Materials/M_GridDecal_Inst.M_GridDecal_Inst'"));
	if (DecalMaterialAsset.Succeeded())
	{
		ActionDecal = DecalMaterialAsset.Object;
	}
	/*static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> MPCA(
		TEXT("MaterialParameterCollection'/Game/Materials/CustomMouseCursorParams.CustomMouseCursorParams'"));
	if (MPCA.Succeeded())
	{*/
	this->MaterialParameterCollectionAsset = FSoftObjectPath(TEXT("/Game/Materials/CustomMouseCursorParams.CustomMouseCursorParams"));
	//}
	static ConstructorHelpers::FClassFinder<UGameplayWidget> WidgetClass(TEXT("/Game/Hud/GwEditorWidget"));
	if (WidgetClass.Succeeded())
	{
		this->GameplayWidgetClass = WidgetClass.Class;
	}
}

void ATopDownCameraController::BeginPlay()
{
	Super::BeginPlay();
	SelectionDecal = GetWorld()->SpawnActor<ASelectionDecalActor>(DragStartPosition, FRotator());
	if (SelectionDecal)
	{
		SelectionDecal->SetDecalMaterial(ActionDecal);
	}
	GameplayWidget = CreateWidget<UGameplayWidget>(this, this->GameplayWidgetClass);
	GameplayWidget->AddToViewport();
	//FindAllActors(GetWorld(), SelectedUnits);
	//FString MyActorName = GetActorLabel();
	//VARDUMP(SelectedUnits.Num(), VARDUMP(FName("SelectedUnits")));
	/*TArray<AGridWorldController*> Temp;
	FindAllActors(GetWorld(), Temp);
	if(!ensure(Temp.Num() >= 1)) return;
	WorldController = Temp[0];*/
}

AGridWorldController* ATopDownCameraController::GetWorldController() const
{
	return WorldController;
}

void ATopDownCameraController::SetWorldController(AGridWorldController* const InWorldController)
{
	this->WorldController = InWorldController;
}

float ATopDownCameraController::GetTileSize() const
{
	return GetWorldController() &&
		GetWorldController()->GetGridWorld() ?
			GetWorldController()->GetGridWorld()->TileWidth : 200.0f;
}

float ATopDownCameraController::GetTileThickness() const
{
	return GetWorldController() &&
		GetWorldController()->GetGridWorld() ?
			GetWorldController()->GetGridWorld()->TileThickness : 200.0f;
}

TArray<ABaseUnitCharacter*>* ATopDownCameraController::GetSelectedUnits()
{
	return &SelectedUnits;
}

void ATopDownCameraController::DeselectUnits()
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

void ATopDownCameraController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	// keep updating the Camera every tick while desired
	if (bManipulateCameraRot)
	{
		float Dx;
		float Dy;
		GetInputMouseDelta(Dx, Dy);
		RotateCamera(Dx*2*10);
		PitchCamera(Dy*2*10);
	}
	else if (bManipulateCamera)
	{
		float Dx;
		float Dy;
		GetInputMouseDelta(Dx, Dy);
		MoveCameraX(-(Dx*2));
		MoveCameraY(-(Dy*2));
	}
	if (bIsDragging)
	{
		WhileDragging();
	}
}

void ATopDownCameraController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("Interact", IE_Pressed, this,
		&ATopDownCameraController::OnInteractPressed);
	InputComponent->BindAction("Interact", IE_Released, this,
	&ATopDownCameraController::OnInteractReleased);
	InputComponent->BindAction("CancelOrExit", IE_Pressed, this,
		&ATopDownCameraController::OnCancelOrExitPressed);
	InputComponent->BindAction("RotateTile", IE_Pressed, this,
		&ATopDownCameraController::OnRotateTiePressed);
	
	InputComponent->BindAction("FocusCamera", IE_Pressed, this,
		&ATopDownCameraController::OnFocusCameraPressed);
	InputComponent->BindAction("ResetCameraRot", IE_Pressed, this,
		&ATopDownCameraController::OnResetCameraRotPressed);
	InputComponent->BindAction("ManipulateCamera", IE_Pressed, this,
		&ATopDownCameraController::OnManipulateCameraPressed);
	InputComponent->BindAction("ManipulateCamera", IE_Released, this,
		&ATopDownCameraController::OnManipulateCameraReleased);
	InputComponent->BindAction("ManipulateCameraRot", IE_Pressed, this,
		&ATopDownCameraController::OnManipulateCameraRotPressed);
	InputComponent->BindAction("ManipulateCameraRot", IE_Released, this,
		&ATopDownCameraController::OnManipulateCameraRotReleased);

	// support touch devices 
	InputComponent->BindTouch(IE_Pressed, this, &ATopDownCameraController::MoveToTouchLocation);
	InputComponent->BindTouch(IE_Repeat, this, &ATopDownCameraController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &ATopDownCameraController::OnResetVR);

	InputComponent->BindAxis("MoveForward", this, &ATopDownCameraController::MoveCameraY);
	InputComponent->BindAxis("MoveRight", this, &ATopDownCameraController::MoveCameraX);
	InputComponent->BindAxis("Pitch", this, &ATopDownCameraController::PitchCamera);
	InputComponent->BindAxis("TurnRate", this, &ATopDownCameraController::RotateCamera);
	
	InputComponent->BindAxis("Zoom", this, &ATopDownCameraController::ZoomCamera);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ATopDownCameraController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATopDownCameraController::InteractUnderMouseCursor()
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
						GetWorldController()->TileClicked(Loc,this->CurrentTileType);
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
						WorldController->InstallWallToTile(Loc,this->CurrentInstalledObjectType);
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

void ATopDownCameraController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
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
}

void ATopDownCameraController::SetNewMoveDestination(const FVector DestLocation)
{
	int Counter = 0;
	const float Sides = FMath::RoundToFloat(FMath::Sqrt(static_cast<float>(GetSelectedUnits()->Num())));
	for (ABaseUnitCharacter* Unit : *GetSelectedUnits())
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


void ATopDownCameraController::RotateTileUnderMouseCursor() const
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
				GetWorldController()->TileRotate(Hit.Location.GridSnap(GetTileSize()));
			}
		}
	}
}

#pragma region CursorDrag
void ATopDownCameraController::StartDrag()
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

void ATopDownCameraController::WhileDragging()
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
					FVector Pos = (Vector - GetWorldController()->GetActorLocation())/GetWorldController()->GetGridWorld()->TileWidth;
					Pos.Z = (Vector.Z - GetWorldController()->GetActorLocation().Z)/GetWorldController()->GetGridWorld()->TileThickness;
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

bool ATopDownCameraController::EndDrag()
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
void ATopDownCameraController::OnFocusCameraPressed()
{
	if (SelectedUnits.Num() > 0)
	{
		if (const ATopDownCameraCharacter* CameraPawn = Cast<ATopDownCameraCharacter>(GetPawn()))
		{
			FVector ActorLocation = SelectedUnits[0]->GetActorLocation();
			ActorLocation.Z = CameraPawn->GetCameraRoot()->GetComponentLocation().Z;
			CameraPawn->GetCameraRoot()->SetWorldLocation(ActorLocation);
		}
	}
}
// ReSharper disable once CppMemberFunctionMayBeConst
void ATopDownCameraController::OnResetCameraRotPressed()
{
	if (const ATopDownCameraCharacter* CameraPawn = Cast<ATopDownCameraCharacter>(GetPawn()))
	{
		CameraPawn->GetCameraBoom()->TargetArmLength = 1600.f;
		CameraPawn->GetCameraBoom()->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	}
}

void ATopDownCameraController::OnManipulateCameraPressed()
{
	bManipulateCamera = true;
}

void ATopDownCameraController::OnManipulateCameraReleased()
{
	bManipulateCamera = false;
}

void ATopDownCameraController::OnManipulateCameraRotPressed()
{
	bManipulateCameraRot = true;
}

void ATopDownCameraController::OnManipulateCameraRotReleased()
{
	bManipulateCameraRot = false;
}

void ATopDownCameraController::OnInteractPressed()
{
	// set flag to keep updating destination until released
	BInteractUnderMouseCursor = true;
	StartDrag();
}

void ATopDownCameraController::OnInteractReleased()
{
	// clear flag to indicate we should stop updating the destination
	BInteractUnderMouseCursor = false;
	if(!EndDrag())
		InteractUnderMouseCursor();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATopDownCameraController::OnRotateTiePressed()
{
	RotateTileUnderMouseCursor();
}

void ATopDownCameraController::OnCancelOrExitPressed()
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

#pragma region Camera
void ATopDownCameraController::MoveCamera(const FVector Vec) const
{
	if (const ATopDownCameraCharacter* CameraPawn = Cast<ATopDownCameraCharacter>(GetPawn()))
	{
		CameraPawn->GetCameraRoot()->AddWorldOffset(Vec);
	}
	
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATopDownCameraController::MoveCameraX(const float X)
{
	if (const ATopDownCameraCharacter* CameraPawn = Cast<ATopDownCameraCharacter>(GetPawn()))
	{
		const FRotator Rot = CameraPawn->GetCameraRoot()->GetComponentRotation();
		const FVector Vec = UKismetMathLibrary::ProjectVectorOnToPlane(
			UKismetMathLibrary::GetRightVector(Rot),
			FVector(0,0,1));
		MoveCamera(Vec.GetSafeNormal() * X*CameraPawn->CameraSpeed);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATopDownCameraController::MoveCameraY(const float Y)
{
	if (const ATopDownCameraCharacter* CameraPawn = Cast<ATopDownCameraCharacter>(GetPawn()))
	{
		const FRotator Rot = CameraPawn->GetCameraRoot()->GetComponentRotation();
		const FVector Vec = UKismetMathLibrary::ProjectVectorOnToPlane(
			UKismetMathLibrary::GetForwardVector(Rot),
			FVector(0,0,1));
		MoveCamera(Vec.GetSafeNormal()  * Y*CameraPawn->CameraSpeed);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATopDownCameraController::RotateCamera(const float Axis)
{
	if (const ATopDownCameraCharacter* CameraPawn = Cast<ATopDownCameraCharacter>(GetPawn()))
	{
		if (const UWorld* World = GetWorld())
		{
			FRotator Rot = CameraPawn->GetCameraRoot()->GetComponentRotation();
			Rot.Yaw = UKismetMathLibrary::ClampAxis( Rot.Yaw + Axis *
				CameraPawn->CameraSpeed * World->GetDeltaSeconds());
			CameraPawn->GetCameraRoot()->MoveComponent(FVector::ZeroVector, Rot, false);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATopDownCameraController::PitchCamera(const float Axis)
{
	if (const ATopDownCameraCharacter* CameraPawn = Cast<ATopDownCameraCharacter>(GetPawn()))
	{
		if (const UWorld* World = GetWorld())
		{
			FRotator Rot = CameraPawn->GetCameraBoom()->GetComponentRotation();
			Rot.Pitch = UKismetMathLibrary::ClampAngle( Rot.Pitch + Axis *
				CameraPawn->CameraSpeed * World->GetDeltaSeconds(),
				-80, -15);
			CameraPawn->GetCameraBoom()->MoveComponent(FVector::ZeroVector, Rot, false);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATopDownCameraController::ZoomCamera(const float Axis)
{
	if (const ATopDownCameraCharacter* CameraPawn = Cast<ATopDownCameraCharacter>(GetPawn()))
	{
		float Len = CameraPawn->GetCameraBoom()->TargetArmLength;
		Len -= Axis * Len;// * World->GetDeltaSeconds();
		CameraPawn->GetCameraBoom()->TargetArmLength = UKismetMathLibrary::Clamp( Len, 300, 5000);
	}
}

#pragma endregion Camera