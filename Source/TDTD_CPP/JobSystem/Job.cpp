// Fill out your copyright notice in the Description page of Project Settings.

#include "Job.h"

#include "GridWorld/Tile.h"

void UJob::Init(UTile* InTile, float InJobTime)
{
	Tile=InTile;
	JobTime=InJobTime;
}

template<typename UserClass>
void UJob::RegisterDone(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(UJob*)>::Type InFunc)
{
	OnJobComplete.AddUObject(InUserObject, InFunc);
	OnJobCancel.AddUObject(InUserObject, InFunc);
}

void UJob::DoWork(float WorkTime)
{
	JobTime -= WorkTime;
	if (JobTime <= 0)
	{
		UE_LOG(LogActor, Display, TEXT("Job's Done! Tile: %s"), *Tile->GetIndexPos().ToCompactString())
		OnJobComplete.Broadcast(this);
	}
}

void UJob::CancelJob()
{
	OnJobCancel.Broadcast(this);
}

#pragma region Callbacks
template<typename UserClass>
FDelegateHandle UJob::RegisterJobCompleteCallback(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(UJob*)>::Type InFunc)
{
	//if (!OnMulti.IsBoundToObject(Del.GetUObject()))
	{
		return OnJobComplete.AddUObject(InUserObject, InFunc);
		//return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("OnJobComplete already Bound, ignoring as I can only have one binding."));
}

void UJob::UnregisterJobCompleteCallback(const void* InUserObject)
{
	if (OnJobComplete.IsBound())
	{
		OnJobComplete.RemoveAll(InUserObject);
	}
}

template<typename UserClass>
FDelegateHandle UJob::RegisterJobCancelCallback(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(UJob*)>::Type InFunc)
{
	//if (!OnMulti.IsBoundToObject(Del.GetUObject()))
	{
		return OnJobCancel.AddUObject(InUserObject, InFunc);
		//return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("OnJobCancel already Bound, ignoring as I can only have one binding."));
}

void UJob::UnregisterJobCancelCallback(const void* InUserObject)
{
	if (OnJobCancel.IsBound())
	{
		OnJobCancel.RemoveAll(InUserObject);
	}
}

template<typename UserClass>
FDelegateHandle UJob::RegisterJobWorkedCallback(UserClass* InUserObject, typename TMemFunPtrType<false, UserClass, void(UJob*)>::Type InFunc)
{
	//if (!OnMulti.IsBoundToObject(Del.GetUObject()))
	{
		return OnJobWorked.AddUObject(InUserObject, InFunc);
		//return;
	}
}

void UJob::UnregisterJobWorkedCallback(const void* InUserObject)
{
	if (OnJobWorked.IsBound())
	{
		OnJobWorked.RemoveAll(InUserObject);
	}
}
#pragma endregion Callbacks