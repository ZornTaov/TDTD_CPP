// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUnitCharacter.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "GridWorld/GridWorldController.h"
#include "GridWorld/Tile.h"
#include "JobSystem/Job.h"

// Sets default values
ABaseUnitCharacter::ABaseUnitCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	//GetCharacterMovement()->bConstrainToPlane = true;
	//GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->MaxStepHeight = 50.0f;

	// Create a decal in the world to show the cursor's location
	SelectionCursor = CreateDefaultSubobject<UDecalComponent>("SelectionCursor");
	SelectionCursor->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(
		TEXT("Material'/Game/Materials/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		SelectionCursor->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	SelectionCursor->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	SelectionCursor->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	SelectionCursor->SetVisibility(false);
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

}

// Called when the game starts or when spawned
void ABaseUnitCharacter::BeginPlay()
{
	Super::BeginPlay();
	//CurrTile = DestTile = GridWorldController->GetGridWorld()->GetTileAtWorldPos(this->GetActorLocation());
	if (BTree)
	{
		AAIController* AiController = GetController<AAIController>();
		AiController->RunBehaviorTree(BTree);
		AIBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);
		AIBlackboard->SetValueAsVector(FName("TargetDestination"), GetActorLocation());
	}
}

// Called every frame
void ABaseUnitCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MyJob)
	{
		MyJob = GridWorldController->GetJobSystem()->GetJob();
		if (MyJob)
		{
			CurrTile = MyJob->GetTile();
			MyJob->OnJobComplete.AddUObject(this, &ABaseUnitCharacter::OnJobEnded);
			MyJob->OnJobCancel.AddUObject(this, &ABaseUnitCharacter::OnJobEnded);
			AIBlackboard->SetValueAsVector(FName("TargetDestination"), CurrTile->GetWorldPos());
		}
	}
}

// Called to bind functionality to input
void ABaseUnitCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseUnitCharacter::OnJobEnded(UJob* Job)
{
	if (Job != MyJob)
	{
		//told about a job that isn't this character's, ignore.
		return;
	}
	
	MyJob = nullptr;
}

