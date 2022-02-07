// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"
#include "BaseUnitCharacter.generated.h"

class AGridWorldController;
class UTile;
class UJob;
UCLASS()
class TDTD_CPP_API ABaseUnitCharacter : public ACharacter
{
	GENERATED_BODY()
#pragma region ACharacter Impl
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Sets default values for this character's properties
	ABaseUnitCharacter();

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* SelectionCursor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBehaviorTree* BTree;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBlackboardComponent* AIBlackboard;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
#pragma endregion ACharacter Impl

#pragma region BaseUnit Fields
protected:

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTile* CurrTile;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTile* DestTile;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UJob* MyJob = nullptr;
	UPROPERTY()
	AGridWorldController* GridWorldController;
#pragma endregion BaseUnit Fields
	
#pragma region BaseUnit Methods
public:
	//UFUNCTION(BlueprintCallable)
	//void Init(UTile* InTile);
	//UFUNCTION(BlueprintCallable)
	//void SetDestination(UTile* InTile);
	void OnJobEnded(UJob* Job);
	
#pragma endregion BaseUnit Methods
};
