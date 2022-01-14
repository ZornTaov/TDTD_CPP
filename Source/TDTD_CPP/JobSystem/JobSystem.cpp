// Fill out your copyright notice in the Description page of Project Settings.

#include "JobSystem.h"

UJob* UJobSystem::MakeJob(UTile* InTile)
{
	UJob* Job = NewObject<UJob>(this);
	Job->Init(InTile, 1.0f);
	JobQueue.Enqueue(Job);
	return Job;
}
