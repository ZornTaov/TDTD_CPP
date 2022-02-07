// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Job.generated.h"

class UJob;
//DECLARE_DELEGATE_OneParam(FOnJobEvent, UJob*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJobEventDispatcher, UJob*);
using FOnJobEvent = FOnJobEventDispatcher::FDelegate;
/**
 * 
 */
UCLASS(BlueprintType)
class TDTD_CPP_API UJob : public UObject
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	class UTile* Tile;
	
public:
	UJob() {  }
	UPROPERTY()
	float JobTime;
	UPROPERTY()
	FName JobName;
	UFUNCTION(BlueprintPure)
	UTile* GetTile() const { return this->Tile; }
	
	FOnJobEventDispatcher OnJobComplete;
	FOnJobEventDispatcher OnJobCancel;
	FOnJobEventDispatcher OnJobWorked;
	void Init(UTile* InTile, float InJobTime = 1.0f);
	template<typename UserClass>
	void RegisterDone(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(UJob*)>::Type InFunc);
	UFUNCTION(BlueprintCallable)
	bool DoWork(float WorkTime);
	void CancelJob();
	
	template<typename UserClass>
	FDelegateHandle RegisterJobCompleteCallback(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(UJob*)>::Type InFunc);
	void UnregisterJobCompleteCallback(const void* InUserObject);
	template<typename UserClass>
	FDelegateHandle RegisterJobCancelCallback(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(UJob*)>::Type InFunc);
	void UnregisterJobCancelCallback(const void* InUserObject);
	template<typename UserClass>
	FDelegateHandle RegisterJobWorkedCallback(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(UJob*)>::Type InFunc);
	void UnregisterJobWorkedCallback(const void* InUserObject);
};
