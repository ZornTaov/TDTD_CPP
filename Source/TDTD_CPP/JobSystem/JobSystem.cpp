// Fill out your copyright notice in the Description page of Project Settings.

#include "JobSystem.h"

UJob* UJobSystem::MakeJob(UTile* InTile)
{
	UJob* Job = NewObject<UJob>(this);
	Job->Init(InTile, 1.0f);
	JobQueue.Enqueue(Job);
	OnJobCreated.Broadcast(Job);
	return Job;
}

UJob* UJobSystem::GetJob()
{
	UJob* Job = nullptr;
	if (!JobQueue.Dequeue(Job))
	{
		return nullptr;
	}
	return Job;
}
