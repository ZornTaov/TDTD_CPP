// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseUnitCharacter.generated.h"

class AGridWorldController;
class UTile;
class UJob;
UCLASS()
class TDTD_CPP_API ABaseUnitCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseUnitCharacter();

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* SelectionCursor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UJob* MyJob = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTile* TargetTile;

	UPROPERTY()
	AGridWorldController* GridWorldController;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnJobEnded(UJob* Job);
};
