// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorld/SelectionDecalActor.h"

#include "Components/DecalComponent.h"

ASelectionDecalActor::ASelectionDecalActor()
{
	this->GetDecal()->DecalSize = FVector(100.0f);
		
	this->AActor::SetActorHiddenInGame(true);
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("SelectionBox"));
	Box->SetBoxExtent(FVector(100.0f));
	Box->SetupAttachment(GetDecal());
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}
