// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Units/BaseUnitCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GridWorld/GridWorldController.h"
#include "GridWorld/SelectionModeEnum.h"
#include "UI/GameplayWidget.h"
#include "TopDownCameraController.generated.h"

class UTDCameraControllerComponent;
class ASelectionDecalActor;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Units, meta = (AllowPrivateAccess = "true"))
	TArray<ABaseUnitCharacter*> SelectedUnits;
	UPROPERTY()
	AGridWorldController* WorldController = nullptr;
	FVector DragStartPosition;
	FVector DragEndPosition;
	TArray<FVector> SelectedTilesLocations;
	bool bIsDragging;
	UPROPERTY()
	ASelectionDecalActor* SelectionDecal = nullptr;
	UPROPERTY()
	UMaterialInstance* ActionDecal;
	UPROPERTY()
	UGameplayWidget* GameplayWidget;
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;
public:
	UPROPERTY()
	TSoftObjectPtr<UMaterialParameterCollection> MaterialParameterCollectionAsset;
	UPROPERTY(BlueprintReadWrite)
	FName CurrentInstalledObjectType;
	UPROPERTY(BlueprintReadWrite)
	ETileType CurrentTileType = ETileType::Ground;
	UPROPERTY(BlueprintReadWrite)
	EGwSelectionMode CurrentMode = EGwSelectionMode::Building;
	AGridWorldController* GetWorldController() const;
	UPROPERTY()
	UTDCameraControllerComponent* CameraController;
	void SetWorldController(AGridWorldController* const InWorldController);
	float GetTileSize() const;
	float GetTileThickness() const;
protected:
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Resets HMD orientation in VR. */
	void OnResetVR();
	void OnWallInstallDone(UJob* Job, FName InstalledObjectType);

	/** Navigate player to the current mouse cursor location. */
	void InteractUnderMouseCursor();
	void RotateTileUnderMouseCursor() const;

	/** Navigate player to the current touch location. */
	//void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);
	
	void OnInteractPressed();
	void OnInteractReleased();
	void OnRotateTiePressed();
	void OnCancelOrExitPressed();
	
	void StartDrag();
	void WhileDragging();
	bool EndDrag();
	
public:
	TArray<ABaseUnitCharacter*>& GetSelectedUnits();
	UFUNCTION(BlueprintCallable)
	void DeselectUnits();
};
