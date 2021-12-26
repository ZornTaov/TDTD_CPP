// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Units/BaseUnitCharacter.h"
#include "GameFramework/PlayerController.h"
#include "TopDownCameraController.generated.h"

UCLASS()
// ReSharper disable once CppUE4CodingStandardNamingViolationWarning
class ATopDownCameraController final : public APlayerController
{
	GENERATED_BODY()

public:
	ATopDownCameraController();
protected:
	virtual void BeginPlay() override;
	/** True if the controlled character should navigate to the mouse cursor. */
	bool bMoveToMouseCursor : 1;
	bool bManipulateCamera;
	bool bManipulateCameraRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Units, meta = (AllowPrivateAccess = "true"))
	TArray<ABaseUnitCharacter*> SelectedUnits;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	void OnManipulateCameraPressed();
	void OnManipulateCameraReleased();
	void OnManipulateCameraRotPressed();
	void OnManipulateCameraRotReleased();
	
	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	
	void MoveCamera(const FVector Vec) const;
	void MoveCameraX(const float X);
	void MoveCameraY(const float Y);
	void RotateCamera(const float Axis);
	void ZoomCamera(const float Axis);
	void PitchCamera(const float Axis);

	TArray<ABaseUnitCharacter*>* GetSelectedUnits();
};
