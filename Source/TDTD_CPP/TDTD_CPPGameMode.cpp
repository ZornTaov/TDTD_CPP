// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDTD_CPPGameMode.h"

#include "EngineUtils.h"
#include "TopDownController.h"
#include "Camera/TopDownCameraCharacter.h"
#include "UObject/ConstructorHelpers.h"

void ATDTD_CPPGameMode::BeginPlay()
{
	Super::BeginPlay();
}

ATDTD_CPPGameMode::ATDTD_CPPGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATopDownController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/TopDownCameraCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
