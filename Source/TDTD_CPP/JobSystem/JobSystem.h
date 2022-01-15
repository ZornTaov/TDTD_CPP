// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Job.h"
#include "Containers/Queue.h"
#include "JobSystem.generated.h"

/**
 * 
 */
UCLASS()
class TDTD_CPP_API UJobSystem : public UObject
{
	TQueue<UJob*> JobQueue;
public:
	FOnJobEventDispatcher OnJobCreated;
	FOnJobEventDispatcher OnJobCompleted;
	
	GENERATED_BODY()
	UJob* MakeJob(UTile* InTile);
};
