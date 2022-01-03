// Fill out your copyright notice in the Description page of Project Settings.


#include "WallTypeComponent.h"

#include "GridWorld.h"
#include "WallStruct.h"


// Sets default values for this component's properties
UWallTypeComponent::UWallTypeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	FillISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FillISM"));
	MiddleISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("MiddleISM"));
	InnerCornerISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InnerCornerISM"));
	OuterCornerISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("OuterCornerISM"));
	FillISM->SetupAttachment(this);
	MiddleISM->SetupAttachment(this);
	InnerCornerISM->SetupAttachment(this);
	OuterCornerISM->SetupAttachment(this);

	WallSubComponents.Add(FillISM);
	WallSubComponents.Add(MiddleISM);
	WallSubComponents.Add(InnerCornerISM);
	WallSubComponents.Add(OuterCornerISM);
	
	// ...
}


// Called when the game starts
void UWallTypeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
	
}


// Called every frame
void UWallTypeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UWallTypeComponent::AddWall(UGridWorld* World, const FVector& InstanceLocation, const bool Propagate) const
{
	TArray<FTile*>* Tiles = World->GetNeighborTiles(InstanceLocation);
	TMap<EWallQuadrant, EWallSubTileType> WallSubTiles = FWallStruct::GetSubTileTypes(*Tiles);
	for (const EWallQuadrant Quadrant : TEnumRange<EWallQuadrant>())
	{
		switch (WallSubTiles[Quadrant]) {
		case Fill:
			FillISM->AddInstance(FTransform(
				FWallStruct::GetQuadrantRotation(Quadrant),
				FWallStruct::GetQuadrantOffset(Quadrant) + InstanceLocation));
			break;
		case InnerCorner:
			InnerCornerISM->AddInstance(FTransform(
				FWallStruct::GetQuadrantRotation(Quadrant),
				FWallStruct::GetQuadrantOffset(Quadrant) + InstanceLocation));
			break;
		case Side:
			MiddleISM->AddInstance(FTransform(
				FWallStruct::GetQuadrantRotation(Quadrant),
				FWallStruct::GetQuadrantOffset(Quadrant) + InstanceLocation));
			break;
		case SideFlipped:
			MiddleISM->AddInstance(FTransform(
				FWallStruct::GetQuadrantRotation(Quadrant) - FRotator(0,90,0),
				FWallStruct::GetQuadrantOffset(Quadrant) + InstanceLocation));
			break;
		case OuterCorner:
			OuterCornerISM->AddInstance(FTransform(
				FWallStruct::GetQuadrantRotation(Quadrant),
				FWallStruct::GetQuadrantOffset(Quadrant) + InstanceLocation));
			break;
		default:
			checkNoEntry();
		}
	}
	if (Propagate)
	{
		UpdateNeighborWalls(World, InstanceLocation);
	}
}

void UWallTypeComponent::AddWalls(UGridWorld* World, const TArray<FVector>& InstanceLocations, bool Propagate) const
{
	for (FVector Location : InstanceLocations)
	{
		AddWall(World, Location, Propagate);
	}
}

void UWallTypeComponent::UpdateNeighborWalls(UGridWorld* World, const FVector& InstanceLocation) const
{
	TArray<FTile*> Tiles = *World->GetNeighborTiles(InstanceLocation);
	TArray<FVector> TileLocations;
	for (const FTile* Tile : Tiles)
	{
		RemoveWall(World, Tile->GetWorldPos(), false);
		TileLocations.Add(Tile->GetWorldPos());
	}
	AddWalls(World, TileLocations, false);
}

void UWallTypeComponent::RemoveWall(UGridWorld* World, const FVector InstanceLocation, const bool Propagate) const
{
	const FTile* Tile = World->GetTileAt(InstanceLocation);
	for (int t = 0; t < WallSubComponents.Num(); ++t)
	{
		TArray<int> Indexes = GetIndex(Tile, t);
		for (const auto Index : Indexes)
		{
			WallSubComponents[t]->RemoveInstance(Index);
		}
	}
	if (Propagate)
	{
		UpdateNeighborWalls(World, InstanceLocation);
	}
}
TArray<int> UWallTypeComponent::GetIndex(const FTile* TileData, const uint8 OldTypeIndex) const
{
	TArray<int> Indexes;
	for (int i = 0; i < WallSubComponents[OldTypeIndex]->GetInstanceCount(); ++i)
	{
		FTransform Transform;
		WallSubComponents[OldTypeIndex]->GetInstanceTransform(i, Transform);
		for (const EWallQuadrant Quadrant : TEnumRange<EWallQuadrant>())
		{
			if (Transform.GetLocation().Equals(TileData->GetWorldPos() + FWallStruct::GetQuadrantOffset(Quadrant)))
			{
				Indexes.Add(i);
			}
		}
	}
	return Indexes;
}
void UWallTypeComponent::ClearInstances()
{
	FillISM->ClearInstances();
	InnerCornerISM->ClearInstances();
	MiddleISM->ClearInstances();
	OuterCornerISM->ClearInstances();
}

int32 UWallTypeComponent::GetInstanceCount() const
{
	return  FillISM->GetInstanceCount() + 
			InnerCornerISM->GetInstanceCount() + 
			MiddleISM->GetInstanceCount() + 
			OuterCornerISM->GetInstanceCount(); 
}

/*void UWallTypeComponent::PartialNavigationUpdate(int32 InstanceIdx)
{
	if (IsValidIndex(InstanceIdx))
	{
		
	}
}*/
