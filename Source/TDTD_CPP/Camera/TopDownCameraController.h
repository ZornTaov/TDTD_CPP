// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "Units/BaseUnitCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GridWorld/GridWorldController.h"
#include "GridWorld/SelectionModeEnum.h"
#include "UI/GameplayWidget.h"
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
	bool BInteractUnderMouseCursor : 1;
	bool bManipulateCamera;
	bool bManipulateCameraRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Units, meta = (AllowPrivateAccess = "true"))
	TArray<ABaseUnitCharacter*> SelectedUnits;
	UPROPERTY()
	AGridWorldController* WorldController = nullptr;
	FVector DragStartPosition;
	FVector DragEndPosition;
	TArray<FVector> SelectedTilesLocations;
	bool bIsDragging;
	UPROPERTY()
	ADecalActor* SelectionDecal = nullptr;
	UPROPERTY()
	UMaterialInterface* ActionDecal;
	UPROPERTY()
	UGameplayWidget* GameplayWidget;
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;
public:
	UPROPERTY(BlueprintReadWrite)
	FName CurrentInstalledObjectType;
	UPROPERTY(BlueprintReadWrite)
	ETileType CurrentTileType = ETileType::Ground;
	UPROPERTY(BlueprintReadWrite)
	EGwSelectionMode CurrentMode = EGwSelectionMode::Building;
	AGridWorldController* GetWorldController() const;
	void SetWorldController(AGridWorldController* const InWorldController);
	float GetTileSize() const;
	float GetTileThickness() const;
protected:
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	void OnRotateTiePressed();
	void RotateTileUnderMouseCursor();
	void OnRotateTieReleased();
	void OnCancelOrExitPressed();
	void OnCancelOrExitReleased();
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Navigate player to the current mouse cursor location. */
	void InteractUnderMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	void OnManipulateCameraPressed();
	void OnManipulateCameraReleased();
	void OnManipulateCameraRotPressed();
	void OnManipulateCameraRotReleased();

	void StartDrag();
	bool EndDrag();
	/** Input handlers for SetDestination action. */
	void OnInteractPressed();
	void OnInteractReleased();
	
	void MoveCamera(const FVector Vec) const;
	void MoveCameraX(const float X);
	void MoveCameraY(const float Y);
	void RotateCamera(const float Axis);
	void ZoomCamera(const float Axis);
	void PitchCamera(const float Axis);

	TArray<ABaseUnitCharacter*>* GetSelectedUnits();
	UFUNCTION(BlueprintCallable)
	void DeselectUnits();
	void WhileDragging();
};
