// Fill out your copyright notice in the Description page of Project Settings.

#include "TileManagerComponent.h"

// Sets default values for this component's properties
UTileManagerComponent::UTileManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UDataTable> Tdt(TEXT("DataTable'/Game/Blueprints/TileInfoTable.TileInfoTable'"));
	if (Tdt.Succeeded())
	{
		FloorTileDataTable = Tdt.Object;
	}
}

// Called when the game starts
void UTileManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UTileManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTileManagerComponent::InitFloorComponents(const USceneComponent* ParentComp)
{
	if (ParentComp->GetNumChildrenComponents() != 0)
	{
		TArray<FTileStruct*> OutTileRowArray;
		FloorTileDataTable->GetAllRows<FTileStruct>(TEXT("GridWorldController#InitFloorComponents"),OutTileRowArray);
		FloorComponents.SetNum(ParentComp->GetNumChildrenComponents());
		for (int i = 0; i < ParentComp->GetNumChildrenComponents(); ++i)
		{
			UInstancedStaticMeshComponent* Child = Cast<UInstancedStaticMeshComponent>(ParentComp->GetChildComponent(i));
			if(IsValid(Child))
			{
				Child->SetStaticMesh(OutTileRowArray[i]->Mesh);	
				FloorComponents[i] = Child;
			}
		}
	}
}

void UTileManagerComponent::InitTileInstance(FTransform TileTransform, UTile* Tile)
{
	const uint8 Type = static_cast<uint8>(Tile->GetType());
	if (Type && FloorComponents.IsValidIndex(Type) && IsValid(FloorComponents[Type]))
	{
		FloorComponents[Type]->AddInstance(TileTransform);
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UTileManagerComponent::ClearTileInstances()
{
	for (UInstancedStaticMeshComponent*& Component : FloorComponents)
	{
		if(Component == nullptr) continue;
		if(IsValid(Component))
			Component->ClearInstances();
	}
}

void UTileManagerComponent::GetIndex(const UTile* TileData, const uint8 OldTypeIndex, int& Index) const
{
	if(FloorComponents.IsValidIndex(OldTypeIndex) && IsValid(FloorComponents[OldTypeIndex]))
	{
		for (int i = 0; i < FloorComponents[OldTypeIndex]->GetInstanceCount(); ++i)
		{
			FTransform Transform;
			FloorComponents[OldTypeIndex]->GetInstanceTransform(i, Transform, true);
			if (Transform.GetLocation().Equals(TileData->GetWorldPos()))
			{
				Index = i;
				break;
			}
		}
	}
}

void UTileManagerComponent::OnTileChanged(UTile* InTile)
{
	ETileType OldType = InTile->GetOldType();
	const uint8 OldTypeIndex = static_cast<uint8>(OldType);
	if (!FloorComponents.IsValidIndex(OldTypeIndex) || !IsValid(FloorComponents[OldTypeIndex]))
	{
		UE_LOG(LogActor, Error, TEXT("Index %d not found for FloorComponents, are we missing a component?"), OldTypeIndex, OldType)
		return;
	}

	const uint8 NewTypeIndex = static_cast<uint8>(InTile->GetType());
	if (!FloorComponents.IsValidIndex(NewTypeIndex) || !IsValid(FloorComponents[NewTypeIndex]))
	{
		UE_LOG(LogActor, Error, TEXT("Index %d not found for FloorComponents, are we missing a component?"), NewTypeIndex, InTile->GetType())
		return;
	}
	//Get correct InstanceIndex
	int Index = -1;
	GetIndex(InTile, OldTypeIndex, Index);
	//Remove from old Type
	if(FloorComponents[OldTypeIndex]->InstanceBodies.IsValidIndex(Index))
	{
		// TODO: Temporary fix for LogNavigationDirtyArea warnings. but not proper solution....
		//FloorComponents[OldTypeIndex]->bNavigationRelevant = FloorComponents[OldTypeIndex]->bNavigationRelevant && FloorComponents[OldTypeIndex]->InstanceBodies.Num() > 1;
		FloorComponents[OldTypeIndex]->RemoveInstance(Index);
	}
	//Add to new Type
	FloorComponents[NewTypeIndex]->AddInstanceWorldSpace(FTransform(InTile->GetRot().Quaternion(), InTile->GetWorldPos()));
}
