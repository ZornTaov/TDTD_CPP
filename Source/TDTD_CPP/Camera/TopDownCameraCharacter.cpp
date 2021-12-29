// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownCameraCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "NavigationSystem.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TopDownCameraController.h"
#include "GridWorld/GridWorld.h"

ATopDownCameraCharacter::ATopDownCameraCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(ACharacter::MeshComponentName))
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(1.0f, 1.0f);
	
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(RootComponent);
	CameraRoot->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraRoot->SetUsingAbsoluteLocation(true);
	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(CameraRoot);
	CameraBoom->TargetArmLength = 1600.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	CameraOffsetter = CreateDefaultSubobject<USceneComponent>(TEXT("CameraOffsetter"));
	CameraOffsetter->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraOffsetter);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	
	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	CursorToWorld->SetUsingAbsoluteLocation(true);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(
		TEXT("Material'/Game/Materials/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATopDownCameraCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATopDownCameraCharacter::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	UpdateCursorPosition(this, CursorToWorld, TopDownCameraComponent);
	
}

void ATopDownCameraCharacter::UpdateCursorPosition(ACharacter* Parent, USceneComponent* Cursor, UCameraComponent* Camera)
{
	if(!ensure(Parent != nullptr)) return;
	if(!ensure(Cursor != nullptr)) return;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Parent);
	FHitResult HitResult;
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (UWorld* World = Parent->GetWorld())
		{
			FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
			FVector StartLocation = Camera->GetComponentLocation();
			FVector EndLocation = Camera->GetComponentRotation().Vector() * 2000.0f;
			Params.AddIgnoredActor(Parent);
			World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
			//FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
			//Cursor->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			Cursor->SetWorldLocation(HitResult.Location);
			//Cursor->SetWorldRotation(SurfaceRotation);
		}
	}
	else if (APlayerController* PC = Cast<APlayerController>(Parent->GetController()))
	{
		PC->GetHitResultUnderCursor(ECC_Visibility, true, HitResult);
		FNavLocation NavLoc;
		FVector QueryingExtent = FVector(5.0f, 5.0f, 250.0f);
		FNavAgentProperties NavAgentProps;
		//Set you NavAgentProps properties here (radius, height, etc)
		NavSys->ProjectPointToNavigation(HitResult.Location, NavLoc, QueryingExtent);
		//DrawDebugPoint(Parent->GetWorld(), NavLoc.Location, 20, FColor(52,220,239), false);

		if (NavLoc.HasNodeRef())
		{
			//FVector CursorFVec = HitResult.ImpactNormal;
			//FRotator CursorR = CursorFVec.Rotation();
			ATopDownCameraController* Controller = Cast<ATopDownCameraController>(PC);
			float GridSnap = Controller ?
				Controller->GetTileSize() : 200.0f;
			FVector CursorL = HitResult.Location.GridSnap(GridSnap);
			CursorL.Z = HitResult.Location.Z;
			Cursor->SetWorldLocation(CursorL);
			//Cursor->SetWorldRotation(CursorR);
		}
	}
}
