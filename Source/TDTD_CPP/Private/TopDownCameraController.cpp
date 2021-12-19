// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownCameraController.h"

#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "TopDownCameraCharacter.h"
#include "BaseUnitCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
template<typename T>
void FindAllActors(UWorld* World, TArray<T*>& Out)
{
	for (TActorIterator<T> It(World); It; ++It)
	{
		Out.Add(*It);
	}
}

ATopDownCameraController::ATopDownCameraController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ATopDownCameraController::BeginPlay()
{
	Super::BeginPlay();
	FindAllActors(GetWorld(), SelectedUnits);
}

void ATopDownCameraController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void ATopDownCameraController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this,
		&ATopDownCameraController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this,
		&ATopDownCameraController::OnSetDestinationReleased);

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

void ATopDownCameraController::MoveToMouseCursor()
{
	if(!ensure(SelectedUnits.Num() != 0)) return;
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
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			FVector CursorL = Hit.ImpactPoint.GridSnap(100.0f);
			CursorL.Z = Hit.ImpactPoint.Z;
			// We hit something, move there
			SetNewMoveDestination(CursorL);
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
	int counter = 0;
	const float sides = FMath::RoundToFloat(FMath::Sqrt(static_cast<float>(GetSelectedUnits()->Num())));
	for (ABaseUnitCharacter* Unit : *GetSelectedUnits())
	{
		int x = FMath::FloorToInt(counter / sides);
		int y = FMath::Fmod(counter, sides);
		FVector pos = DestLocation+FVector(x*100, y*100, 0);
		if (Unit)
		{
			float const Distance = FVector::Dist(pos, Unit->GetActorLocation());

			AAIController* Controller = Unit->GetController<AAIController>();
			if(!ensure(Controller != nullptr)) return;
			// We need to issue move command only if far enough in order for walk animation to play correctly
			if ((Distance > 120.0f))
			{
				Controller->MoveToLocation(pos, 50, true, true,
					false, false, nullptr, true);
				//UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, pos);
			}
		}
		counter++;
	}
}

void ATopDownCameraController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ATopDownCameraController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

TArray<ABaseUnitCharacter*>* ATopDownCameraController::GetSelectedUnits()
{
	return &SelectedUnits;
}

#pragma region Camera
void ATopDownCameraController::MoveCamera(const FVector Vec) const
{
	if (const ATopDownCameraCharacter* CameraPawn = Cast<ATopDownCameraCharacter>(GetPawn()))
	{
		CameraPawn->GetCameraBoom()->MoveComponent(Vec, CameraPawn->GetCameraBoom()->GetComponentRotation(), false);
	}
	
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ATopDownCameraController::MoveCameraX(const float X)
{
	if (const ATopDownCameraCharacter* CameraPawn = Cast<ATopDownCameraCharacter>(GetPawn()))
	{
		const FRotator Rot = CameraPawn->GetCameraBoom()->GetComponentRotation();
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
		const FRotator Rot = CameraPawn->GetCameraBoom()->GetComponentRotation();
		const FVector Vec = UKismetMathLibrary::ProjectVectorOnToPlane(
			UKismetMathLibrary::GetUpVector(Rot),
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
			FRotator Rot = CameraPawn->GetCameraBoom()->GetComponentRotation();
			Rot.Yaw = UKismetMathLibrary::ClampAxis( Rot.Yaw + Axis *
				CameraPawn->CameraSpeed * World->GetDeltaSeconds());
			CameraPawn->GetCameraBoom()->MoveComponent(FVector::ZeroVector, Rot, false);
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
			FRotator Rot = CameraPawn->GetCameraOffsetter()->GetComponentRotation();
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
		if (const UWorld* World = GetWorld())
		{
			float Len = CameraPawn->GetCameraBoom()->TargetArmLength;
			Len += Axis * 2000 * World->GetDeltaSeconds();
			CameraPawn->GetCameraBoom()->TargetArmLength = UKismetMathLibrary::Clamp( Len, 500, 2000);
		}
	}
}
#pragma endregion Camera
