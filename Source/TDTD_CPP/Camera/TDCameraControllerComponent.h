// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TopDownCameraCharacter.h"
#include "TopDownController.h"
#include "Components/ActorComponent.h"
#include "TDCameraControllerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TDTD_CPP_API UTDCameraControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTDCameraControllerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY()
	ATopDownController* Controller;
	UPROPERTY()
	ATopDownCameraCharacter* CameraPawn;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	
	/** Input handlers for interaction. */
	bool bManipulateCamera;
	bool bManipulateCameraRot;
	void OnFocusCameraPressed();
	void OnResetCameraRotPressed();
	void OnManipulateCameraPressed();
	void OnManipulateCameraReleased();
	void OnManipulateCameraRotPressed();
	void OnManipulateCameraRotReleased();
	void MoveCamera(const FVector Vec) const;
	void MoveCameraX(const float X);
	void MoveCameraY(const float Y);
	void RotateCamera(const float Axis);
	void ZoomCamera(const float Axis);
	void PitchCamera(const float Axis);
};
