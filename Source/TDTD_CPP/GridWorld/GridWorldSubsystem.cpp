// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldSubsystem.h"

AGridWorldController* UGridWorldSubsystem::GetGridWorldController() const
{
	return Gwc;
}

void UGridWorldSubsystem::SetGridWorldController(AGridWorldController* const InGwc)
{
	this->Gwc = InGwc;
}

UGridWorld* UGridWorldSubsystem::GetGridWorld() const
{
	return Gw;
}

void UGridWorldSubsystem::SetGridWorld(UGridWorld* const InGw)
{
	this->Gw = InGw;
}
