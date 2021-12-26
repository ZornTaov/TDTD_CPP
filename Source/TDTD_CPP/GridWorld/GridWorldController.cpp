// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldController.h"

#include "GridWorld.h"
#include "VarDump.h"

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


// Called when the game starts or when spawned
void AGridWorldController::BeginPlay()
{
	Super::BeginPlay();
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
				FTile* Tile = World->GetTileAt(x, y, z);
				TileTransform.SetLocation(FVector(x * World->TileSize, y * World->TileSize, z * World->TileThickness));
				const uint8 Type = static_cast<uint8>(Tile->GetType());
				if (Type && Type < FloorComponents.Num())
				{
					Tile->InstanceIndex = FloorComponents[Type]->AddInstance(TileTransform);
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

void AGridWorldController::OnTileTypeChanged(FTile& TileDataRef, ETileType NewType) const
{
	FTile* TileData = &TileDataRef;
	ETileType OldType = TileData->GetType();
	if (!FloorComponents.IsValidIndex(static_cast<uint8>(OldType)))
	{
		UE_LOG(LogActor, Error, TEXT("Index %d not found for %s, are we missing a component?"), TileData->GetInstanceIndex(), OldType)
		return;
	} 
	if(FloorComponents[static_cast<uint8>(OldType)]->GetNumRenderInstances() <= TileData->GetInstanceIndex())
		FloorComponents[static_cast<uint8>(OldType)]->RemoveInstance(TileData->InstanceIndex);
	
	if (!FloorComponents.IsValidIndex(static_cast<uint8>(NewType)))
	{
		UE_LOG(LogActor, Error, TEXT("Index %d not found for %s, are we missing a component?"), TileData->GetInstanceIndex(), NewType)
		return;
	}
	const int Id = FloorComponents[static_cast<uint8>(NewType)]->AddInstance(FTransform(TileData->GetRot().Quaternion(), TileData->GetWorldPos()));
	TileData->SetInstanceIndex(Id);
}

FTile* AGridWorldController::UpdateTile(const FVector Pos, const ETileType& NewType) const
{
	return UpdateTile(Pos.X, Pos.Y, Pos.Z, NewType);
}

FTile* AGridWorldController::UpdateTile(const int X, const int Y, const int Z, const ETileType NewType) const
{
	FTile* Tile= World->GetTileAt(X, Y, Z);
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
