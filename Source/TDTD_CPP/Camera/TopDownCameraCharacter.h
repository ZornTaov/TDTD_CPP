// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TopDownCameraCharacter.generated.h"

class UCameraComponent;
UCLASS(Blueprintable)
class ATopDownCameraCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

private:
	static void UpdateCursorPosition(ACharacter* Parent, USceneComponent* Cursor, UCameraComponent* Camera);
	
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* CameraOffsetter;
	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* CameraRoot;
	
public:
	virtual void BeginPlay() override;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent sub-object **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraRoot sub-object **/
	FORCEINLINE class USceneComponent* GetCameraRoot() const { return CameraRoot; }
	/** Returns CameraBoom sub-object **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CameraBoom sub-object **/
	FORCEINLINE class USceneComponent* GetCameraOffsetter() const { return CameraOffsetter; }
	/** Returns SelectionCursor sub-object **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() const { return CursorToWorld; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraSpeed = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FRotator DefaultRotation = FRotator(-60.f, 0.f, 0.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float DefaultZoom = 1600.f;
};

