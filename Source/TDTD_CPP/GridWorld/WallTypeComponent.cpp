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
	GhostPrototypeISM = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("GhostPrototypeISM"));
	FillISM->SetupAttachment(this);
	MiddleISM->SetupAttachment(this);
	InnerCornerISM->SetupAttachment(this);
	OuterCornerISM->SetupAttachment(this);
	GhostPrototypeISM->SetupAttachment(this);

	WallSubComponents.Add(FillISM);
	WallSubComponents.Add(MiddleISM);
	WallSubComponents.Add(InnerCornerISM);
	WallSubComponents.Add(OuterCornerISM);
	
	// ...
}

void UWallTypeComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	// Destroy ISM's anyways
	for (auto Component : this->WallSubComponents)
	{
		if(Component)
			Component->DestroyComponent();
	}
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


void UWallTypeComponent::AddWall(const UTile* InTile, const bool Propagate) const
{
	TArray<UTile*> Tiles = InTile->World->GetNeighborTiles(InTile->GetIndexPos());
	const FVector CenterLocation = Tiles[static_cast<int8>(ETileDirection::Center)]->GetWorldPos();
	TMap<EWallQuadrant, EWallSubTileType> WallSubTiles = FWallStruct::GetSubTileTypes(Tiles);
	for (const EWallQuadrant Quadrant : TEnumRange<EWallQuadrant>())
	{
		switch (WallSubTiles[Quadrant]) {
		case Fill:
			FillISM->AddInstanceWorldSpace(FTransform(
				FWallStruct::GetQuadrantRotation(Quadrant),
				FWallStruct::GetQuadrantOffset(Quadrant) + CenterLocation));
			break;
		case InnerCorner:
			InnerCornerISM->AddInstanceWorldSpace(FTransform(
				FWallStruct::GetQuadrantRotation(Quadrant),
				FWallStruct::GetQuadrantOffset(Quadrant) + CenterLocation));
			break;
		case Side:
			MiddleISM->AddInstanceWorldSpace(FTransform(
				FWallStruct::GetQuadrantRotation(Quadrant) - FRotator(0,90,0),
				FWallStruct::GetQuadrantOffset(Quadrant) + CenterLocation));
			break;
		case SideFlipped:
			MiddleISM->AddInstanceWorldSpace(FTransform(
				FWallStruct::GetQuadrantRotation(Quadrant),
				FWallStruct::GetQuadrantOffset(Quadrant) + CenterLocation));
			break;
		case OuterCorner:
			OuterCornerISM->AddInstanceWorldSpace(FTransform(
				FWallStruct::GetQuadrantRotation(Quadrant),
				FWallStruct::GetQuadrantOffset(Quadrant) + CenterLocation));
			break;
		default:
			checkNoEntry();
		}
	}
	if (Propagate)
	{
		UpdateNeighborWalls(InTile);
	}
}

void UWallTypeComponent::AddWalls(UGridWorld* World, const TArray<FVector>& InstanceLocations, bool Propagate) const
{
	for (FVector Location : InstanceLocations)
	{
		const UTile* Tile = World->GetTileAt(Location);
		AddWall(Tile, Propagate);
	}
}

void UWallTypeComponent::UpdateNeighborWalls(const UTile* InTile) const
{
	TArray<UTile*> Tiles = InTile->World->GetNeighborTiles(InTile->GetIndexPos());
	TArray<FVector> TilesToUpdate;
	for (const UTile* Tile : Tiles)
	{
		if (Tile && Tile->InstalledObject != nullptr)
		{
			RemoveWall(Tile, false);
			TilesToUpdate.Add(Tile->GetIndexPos());
		}
	}
	AddWalls(InTile->World, TilesToUpdate, false);
}

void UWallTypeComponent::RemoveWall(const UTile* InTile, const bool Propagate) const
{
	for (int t = 0; t < WallSubComponents.Num(); ++t)
	{
		TArray<int> Indexes = GetIndex(InTile, t);
		for (const auto Index : Indexes)
		{
			WallSubComponents[t]->RemoveInstance(Index);
		}
	}
	if (Propagate)
	{
		UpdateNeighborWalls(InTile);
	}
}
TArray<int> UWallTypeComponent::GetIndex(const UTile* TileData, const uint8 OldTypeIndex) const
{
	TArray<int> Indexes;
	for (int i = 0; i < WallSubComponents[OldTypeIndex]->GetInstanceCount(); ++i)
	{
		FTransform Transform;
		WallSubComponents[OldTypeIndex]->GetInstanceTransform(i, Transform, true);
		for (const EWallQuadrant Quadrant : TEnumRange<EWallQuadrant>())
		{
			if (Transform.GetLocation().Equals(TileData->GetWorldPos() + FWallStruct::GetQuadrantOffset(Quadrant), 10))
			{
				Indexes.Add(i);
			}
		}
	}
	Algo::Sort(Indexes,[](int A, int B){return A > B;});
	return Indexes;
}
void UWallTypeComponent::ClearInstances()
{
	FillISM->ClearInstances();
	InnerCornerISM->ClearInstances();
	MiddleISM->ClearInstances();
	OuterCornerISM->ClearInstances();
	GhostPrototypeISM->ClearInstances();
}

int32 UWallTypeComponent::GetInstanceCount() const
{
	return  FillISM->GetInstanceCount() + 
			InnerCornerISM->GetInstanceCount() + 
			MiddleISM->GetInstanceCount() + 
			OuterCornerISM->GetInstanceCount(); 
}

void UWallTypeComponent::AddGhostWall(const UTile* InTile) const
{
	const FVector Loc = InTile->GetWorldPos();
	const int GhostIndex = GetGhostIndex(InTile);
	if (GhostPrototypeISM->InstanceBodies.IsValidIndex(GhostIndex))
	{
		UE_LOG(LogTile, Warning, TEXT("Ghost at %s already exists, did you mean to add here?"),
			*Loc.ToCompactString());
		return;
	}
	GhostPrototypeISM->AddInstanceWorldSpace(FTransform(Loc));
}

void UWallTypeComponent::RemoveGhostWall(const UTile* InTile) const
{
	const int GhostIndex = GetGhostIndex(InTile);
	if (!GhostPrototypeISM->InstanceBodies.IsValidIndex(GhostIndex))
	{
		UE_LOG(LogTile, Warning, TEXT("Ghost at %s does not exist, did you mean to remove here?"),
			*InTile->GetWorldPos().ToCompactString());
		return;
	}
	GhostPrototypeISM->RemoveInstance(GhostIndex);
	
}

int UWallTypeComponent::GetGhostIndex(const UTile* InTile) const
{
	for (int i = 0; i < GhostPrototypeISM->GetInstanceCount(); ++i)
	{
		FTransform Transform;
		GhostPrototypeISM->GetInstanceTransform(i, Transform, true);
		if (Transform.GetLocation().Equals(InTile->GetWorldPos(), 10))
		{
			return i;
		}
	}
	return -1;
}

/*void UWallTypeComponent::PartialNavigationUpdate(int32 InstanceIdx)
{
	if (IsValidIndex(InstanceIdx))
	{
		
	}
}*/

