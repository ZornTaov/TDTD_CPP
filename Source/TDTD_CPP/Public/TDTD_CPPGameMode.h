// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TDTD_CPPGameMode.generated.h"

UCLASS(minimalapi)
class ATDTD_CPPGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:
	ATDTD_CPPGameMode();
};



