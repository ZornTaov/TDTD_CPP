// Fill out your copyright notice in the Description page of Project Settings.

#include "TDCameraControllerComponent.h"

#include "TopDownCameraCharacter.h"
#include "TopDownCameraController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UTDCameraControllerComponent::UTDCameraControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTDCameraControllerComponent::BeginPlay()
{
	Super::BeginPlay();
	Controller = GetOwner<ATopDownCameraController>();
	CameraPawn = Controller->GetPawn<ATopDownCameraCharacter>();
}

// Called every frame
void UTDCameraControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Controller)
	{
		// keep updating the Camera every tick while desired
		if (bManipulateCameraRot)
		{
			float Dx;
			float Dy;
			Controller->GetInputMouseDelta(Dx, Dy);
			RotateCamera(Dx * 2 * 10);
			PitchCamera(Dy * 2 * 10);
		}
		else if (bManipulateCamera)
		{
			float Dx;
			float Dy;
			Controller->GetInputMouseDelta(Dx, Dy);
			MoveCameraX(-(Dx * 2));
			MoveCameraY(-(Dy * 2));
		}
	}
}

#pragma region OnPressedReleased
// ReSharper disable once CppMemberFunctionMayBeConst
void UTDCameraControllerComponent::OnFocusCameraPressed()
{
	if (Controller)
	{
		if (Controller->GetSelectedUnits().Num() > 0)
		{
			if (CameraPawn)
			{
				FVector ActorLocation = Controller->GetSelectedUnits()[0]->GetActorLocation();
				ActorLocation.Z = CameraPawn->GetCameraRoot()->GetComponentLocation().Z;
				CameraPawn->GetCameraRoot()->SetWorldLocation(ActorLocation);
			}
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UTDCameraControllerComponent::OnResetCameraRotPressed()
{
	if (CameraPawn)
	{
		CameraPawn->GetCameraBoom()->TargetArmLength = CameraPawn->DefaultZoom;
		CameraPawn->GetCameraBoom()->SetRelativeRotation(CameraPawn->DefaultRotation);
		CameraPawn->GetCameraRoot()->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

void UTDCameraControllerComponent::OnManipulateCameraPressed()
{
	bManipulateCamera = true;
}

void UTDCameraControllerComponent::OnManipulateCameraReleased()
{
	bManipulateCamera = false;
}

void UTDCameraControllerComponent::OnManipulateCameraRotPressed()
{
	bManipulateCameraRot = true;
}

void UTDCameraControllerComponent::OnManipulateCameraRotReleased()
{
	bManipulateCameraRot = false;
}
#pragma endregion OnPressedReleased

#pragma region Camera
void UTDCameraControllerComponent::MoveCamera(const FVector Vec) const
{
	if (CameraPawn)
	{
		CameraPawn->GetCameraRoot()->AddWorldOffset(Vec);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UTDCameraControllerComponent::MoveCameraX(const float X)
{
	if (CameraPawn)
	{
		const FRotator Rot = CameraPawn->GetCameraRoot()->GetComponentRotation();
		const FVector Vec = UKismetMathLibrary::ProjectVectorOnToPlane(
			UKismetMathLibrary::GetRightVector(Rot),
			FVector(0, 0, 1));
		MoveCamera(Vec.GetSafeNormal() * X * CameraPawn->CameraSpeed);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UTDCameraControllerComponent::MoveCameraY(const float Y)
{
	if (CameraPawn)
	{
		const FRotator Rot = CameraPawn->GetCameraRoot()->GetComponentRotation();
		const FVector Vec = UKismetMathLibrary::ProjectVectorOnToPlane(
			UKismetMathLibrary::GetForwardVector(Rot),
			FVector(0, 0, 1));
		MoveCamera(Vec.GetSafeNormal() * Y * CameraPawn->CameraSpeed);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UTDCameraControllerComponent::RotateCamera(const float Axis)
{
	if (CameraPawn)
	{
		if (const UWorld* World = GetWorld())
		{
			FRotator Rot = CameraPawn->GetCameraRoot()->GetComponentRotation();
			Rot.Yaw = UKismetMathLibrary::ClampAxis(Rot.Yaw + Axis *
				CameraPawn->CameraSpeed * World->GetDeltaSeconds());
			CameraPawn->GetCameraRoot()->MoveComponent(FVector::ZeroVector, Rot, false);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UTDCameraControllerComponent::PitchCamera(const float Axis)
{
	if (CameraPawn)
	{
		if (const UWorld* World = GetWorld())
		{
			FRotator Rot = CameraPawn->GetCameraBoom()->GetComponentRotation();
			Rot.Pitch = UKismetMathLibrary::ClampAngle(Rot.Pitch + Axis *
			                                           CameraPawn->CameraSpeed * World->GetDeltaSeconds(),
			                                           -80, -15);
			CameraPawn->GetCameraBoom()->MoveComponent(FVector::ZeroVector, Rot, false);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UTDCameraControllerComponent::ZoomCamera(const float Axis)
{
	if (CameraPawn)
	{
		float Len = CameraPawn->GetCameraBoom()->TargetArmLength;
		Len -= Axis * Len; // * World->GetDeltaSeconds();
		CameraPawn->GetCameraBoom()->TargetArmLength = UKismetMathLibrary::Clamp(Len, 300, 5000);
	}
}
#pragma endregion Camera
