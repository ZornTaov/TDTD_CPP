// Fill out your copyright notice in the Description page of Project Settings.

#include "WallManagerComponent.h"

#include "WallStruct.h"
#include "WallTypeComponent.h"
#include "Installables/InstalledObject.h"

// Sets default values for this component's properties
UWallManagerComponent::UWallManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UDataTable> Wdt(TEXT("DataTable'/Game/Blueprints/WallInfoTable.WallInfoTable'"));
	if (Wdt.Succeeded())
	{
		WallTileDataTable = Wdt.Object;
	}
}

// Called when the game starts
void UWallManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UWallManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UWallManagerComponent::ClearWallInstances()
{
	for (UWallTypeComponent*& Component : WallComponents)
	{
		if(Component == nullptr) continue;
		if(IsValid(Component))
			Component->ClearInstances();
	}
}

void UWallManagerComponent::PlaceWall(UTile* TileAt, FName NameToCheck, bool Remove)
{
	const UWallTypeComponent* WallPlacer = nullptr;
	UWallTypeComponent** Res = WallComponents.FindByPredicate([NameToCheck](const UWallTypeComponent* WallType){return WallType->WallTypeName == NameToCheck;});
	if (Res && *Res)
	{
		WallPlacer = *Res;
	}
	if (IsValid(WallPlacer))
	{
		if (Remove)
		{
			WallPlacer->RemoveWall(TileAt);
		}
		else
		{
			WallPlacer->RemoveGhostWall(TileAt);
			WallPlacer->AddWall(TileAt);
		}
	}
}

void UWallManagerComponent::PlaceGhostWall(UTile* TileAt, const FName& InstalledObjectName)
{
	const UWallTypeComponent* WallPlacer = nullptr;
	UWallTypeComponent** Res = WallComponents.FindByPredicate([InstalledObjectName](const UWallTypeComponent* WallType){return WallType->WallTypeName == InstalledObjectName;});
	if (Res && *Res)
	{
		WallPlacer = *Res;
	}
	if (IsValid(WallPlacer))
	{
		WallPlacer->AddGhostWall(TileAt);
	}
}

void UWallManagerComponent::InitWallInstance(UTile* Tile)
{
	if (Tile->InstalledObject)
	{
		PlaceWall(Tile, Tile->InstalledObject->ObjectType);
	}
}

void UWallManagerComponent::InitWallComponents(const USceneComponent* ParentComp)
{
	if (ParentComp->GetNumChildrenComponents() != 0)
	{
		TArray<FWallStruct*> OutWallRowArray;
		WallTileDataTable->GetAllRows<FWallStruct>(TEXT("GridWorldController#InitWallComponents"),OutWallRowArray);
		if (OutWallRowArray.Num() == 0)
		{
			return;
		}
		WallComponents.SetNum(ParentComp->GetNumChildrenComponents());
		for (int i = 0; i < ParentComp->GetNumChildrenComponents(); ++i)
		{
			UWallTypeComponent* Child = Cast<UWallTypeComponent>(ParentComp->GetChildComponent(i));
			if(IsValid(Child))
			{
				Child->FillISM->SetStaticMesh(OutWallRowArray[i]->FillMesh);
				Child->InnerCornerISM->SetStaticMesh(OutWallRowArray[i]->InnerCornerMesh);
				Child->MiddleISM->SetStaticMesh(OutWallRowArray[i]->MiddleMesh);
				Child->OuterCornerISM->SetStaticMesh(OutWallRowArray[i]->OuterCornerMesh);
				Child->WallTypeName = WallTileDataTable->GetRowNames()[i];
				Child->GhostPrototypeISM->SetStaticMesh(OutWallRowArray[i]->GhostPrototype);
				WallComponents[i] = Child;
			}
		}
	}
}
