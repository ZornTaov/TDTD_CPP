// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldController.h"

#include "GridWorld.h"
#include "VarDump.h"
#include "Camera/TopDownCameraController.h"

// Sets default values
AGridWorldController::AGridWorldController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	World = CreateDefaultSubobject<UGridWorld>(TEXT("GridWorld"));
	AddOwnedComponent(World);

	WorldRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WorldRootComponent"));
	RootComponent = WorldRootComponent;
	FloorsRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FloorsRootComponent"));
	FloorsRootComponent->SetupAttachment(WorldRootComponent);
	WallsRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WallsRootComponent"));
	WallsRootComponent->SetupAttachment(WorldRootComponent);
	
	static ConstructorHelpers::FObjectFinder<UDataTable> Tdt(TEXT("DataTable'/Game/Blueprints/TileInfoTable.TileInfoTable'"));
    if (Tdt.Succeeded())
    {
	    FloorTileDataTable = Tdt.Object;
    }
}

UGridWorld* AGridWorldController::GetGridWorld() const
{
	return World;
}

void AGridWorldController::SetGridWorld(UGridWorld* const InWorld)
{
	this->World = InWorld;
}


// Called when the game starts or when spawned
void AGridWorldController::BeginPlay()
{
	Super::BeginPlay();
	ATopDownCameraController* Controller = Cast<ATopDownCameraController>(GetWorld()->GetFirstPlayerController());
	if (Controller)
	{
		Controller->SetWorldController(this);
	}
	//World->Init();
	/*int Layer = 0;
	for (int x = 0; x < World->Size().X; ++x)
	{
		for (int y = 0; y < World->Size().Y; ++y)
		{
			FTile* Tile_Data = World->GetTileAt(x, y, 0);
			FActorSpawnParameters Params = FActorSpawnParameters();
			Params.Name = FName(FString::Printf(TEXT("tile_%d_%d"), x, y));
	    	
			Params.Owner = this;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FVector Location = FVector(x, y, Layer)*World->TileSize;
			ATileActor* TileActor = AActor::GetWorld()->SpawnActor<ATileActor>(Location, FRotator::ZeroRotator, Params);
			//Tile_Data->Del.AddLambda([TileActor, this](FTile* Tile){OnTileTypeChanged(*Tile, this->FloorTileDataTable, TileActor);});
			Tile_Data->SetType(ETileType::Floor);
		}
	}
	GetWorld()->GetNavigationSystem()->InitializeForWorld(*GetWorld(), FNavigationSystemRunMode::GameMode);*/
}

void AGridWorldController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	InitComponents(FloorComponents, FloorsRootComponent, FloorTileDataTable);
	//InitComponents(WallComponents, WallsRootComponent, WallTileDataTable);
	World->Init(ETileType::Ground);
	ClearAllInstances();
	InitInstance();
}

void AGridWorldController::InitInstance()
{
	FTransform TileTransform;
	TileTransform.SetRotation(FQuat::Identity);
	for (int z = 0; z < World->Depth; ++z)
	{
		for (int x = 0; x < World->Size().X; ++x)
		{
			for (int y = 0; y < World->Size().Y; ++y)
			{
				const FTile* Tile = World->GetTileAt(x, y, z);
				TileTransform.SetLocation(FVector(x * World->TileSize, y * World->TileSize, z * World->TileThickness));
				const uint8 Type = static_cast<uint8>(Tile->GetType());
				if (Type && Type < FloorComponents.Num())
				{
					FloorComponents[Type]->AddInstance(TileTransform);
				}
				/*const uint8 WallType = static_cast<uint8>(Tile->GetWallType());
				if (WallType && WallType < WallComponents.Num())
				{
					Tile->InstanceIndex = WallComponents[WallType]->AddInstance(TileTransform);
				}*/
			}
		}
	}
}

void AGridWorldController::InitComponents(TArray<UInstancedStaticMeshComponent*>& Components, const USceneComponent* ParentComp, const UDataTable* Data) const
{
	if (ParentComp->GetNumChildrenComponents() != 0)
	{
		TArray<FTileStruct*> OutRowArray;
		Data->GetAllRows<FTileStruct>(TEXT("GridWorldController#InitComponents"),OutRowArray);
		Components.SetNum(ParentComp->GetNumChildrenComponents());
		for (int i = 0; i < ParentComp->GetNumChildrenComponents(); ++i)
		{
			UInstancedStaticMeshComponent* Child = Cast<UInstancedStaticMeshComponent>(ParentComp->GetChildComponent(i));
			if(IsValid(Child))
			{
				Child->SetStaticMesh(OutRowArray[i]->Mesh);
				Components[i] = Child;
			}
		}
	}
}

void AGridWorldController::ClearAllInstances()
{
	ClearInstances(FloorComponents);
	//ClearInstances(WallComponents);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void AGridWorldController::ClearInstances(TArray<UInstancedStaticMeshComponent*> &Components)
{
	for (UInstancedStaticMeshComponent*& Component : Components)
	{
		if(Component == nullptr) continue;
		if(IsValid(Component))
			Component->ClearInstances();
	}
}

void AGridWorldController::TileClicked(const FVector& Vector) const
{
	FVector Pos = (Vector-GetActorLocation())/World->TileSize;
	Pos.Z = (Vector.Z - GetActorLocation().Z)/World->TileThickness;
	FTile* Tile = World->GetTileAt(Pos);
	if (!Tile)
	{
		return;
	}
	// TODO: Temporary switch/case to rotate TileType
	switch (Tile->GetType())
	{
	case ETileType::Ground:
		UpdateTile(Pos, ETileType::Floor, Tile);
		break;
	case ETileType::Floor:
		UpdateTile(Pos, ETileType::Ramp, Tile);
		break;
	case ETileType::Ramp:
		UpdateTile(Pos, ETileType::Ground, Tile);
		break;
	case ETileType::Empty: 
	default:
		checkNoEntry();
	}
}

void AGridWorldController::TileRotate(const FVector& Vector) const
{
	FVector Pos = (Vector-GetActorLocation())/World->TileSize;
	Pos.Z = (Vector.Z - GetActorLocation().Z)/World->TileThickness;
	FTile* Tile = World->GetTileAt(Pos);
	if (!Tile)
	{
		return;
	}
	Tile->SetRot(Tile->GetRot()+FRotator(0,90,0));
	UpdateTile(Pos, Tile->GetType(), Tile);
}

void AGridWorldController::OnTileTypeChanged(const FTile& TileDataRef, ETileType NewType) const
{
	const FTile* TileData = &TileDataRef;
	ETileType OldType = TileData->GetType();
	const uint8 OldTypeIndex = static_cast<uint8>(OldType);
	if (!FloorComponents.IsValidIndex(OldTypeIndex))
	{
		UE_LOG(LogActor, Error, TEXT("Index %d not found for FloorComponents, are we missing a component?"), OldTypeIndex, OldType)
		return;
	}

	const uint8 NewTypeIndex = static_cast<uint8>(NewType);
	if (!FloorComponents.IsValidIndex(NewTypeIndex))
	{
		UE_LOG(LogActor, Error, TEXT("Index %d not found for FloorComponents, are we missing a component?"), NewTypeIndex, NewType)
		return;
	}
	//Get correct InstanceIndex
	int Index = -1;
	for (int i = 0; i < FloorComponents[OldTypeIndex]->GetInstanceCount(); ++i)
	{
		FTransform Transform;
		FloorComponents[OldTypeIndex]->GetInstanceTransform(i, Transform);
		if ((Transform.GetLocation()).Equals(TileData->GetWorldPos()))
		{
			Index = i;
			break;
		}
	}
	//Remove from old Type
	if(FloorComponents[OldTypeIndex]->InstanceBodies.IsValidIndex(Index))
	{
		// TODO: Temporary fix for LogNavigationDirtyArea warnings. but not proper solution....
		FloorComponents[OldTypeIndex]->bNavigationRelevant = FloorComponents[OldTypeIndex]->bNavigationRelevant && FloorComponents[OldTypeIndex]->InstanceBodies.Num() > 1;
		FloorComponents[OldTypeIndex]->RemoveInstance(Index);
	}
	//Add to new Type
	FloorComponents[NewTypeIndex]->AddInstance(FTransform(TileData->GetRot().Quaternion(), TileData->GetWorldPos()));
}

FTile* AGridWorldController::UpdateTile(const FVector Pos, const ETileType& NewType, FTile* Tile) const
{
	return UpdateTile(Pos.X, Pos.Y, Pos.Z, NewType, Tile);
}

FTile* AGridWorldController::UpdateTile(const int X, const int Y, const int Z, const ETileType NewType, FTile* InTile) const
{
	FTile* Tile = InTile;
	if (!Tile)
	{
		Tile = World->GetTileAt(X, Y, Z);
	}
	OnTileTypeChanged(*Tile, NewType);
	Tile->SetType(NewType);
	return Tile;
}

// Called every frame
// ReSharper disable once CppParameterMayBeConst
void AGridWorldController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
