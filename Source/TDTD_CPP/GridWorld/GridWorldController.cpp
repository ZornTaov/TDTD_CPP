// Fill out your copyright notice in the Description page of Project Settings.

#include "GridWorldController.h"

#include "DrawDebugHelpers.h"
#include "GridWorld.h"
#include "Installables/InstalledObject.h"
#include "TDTDExtensionHelpers.h"
#include "TDTDDebugHelpers.h"
#include "TDTD_CPP.h"
#include "TileManagerComponent.h"
#include "VarDump.h"
#include "TopDownController.h"
#include "WallManagerComponent.h"
#include "Engine/AssetManager.h"
#include "JobSystem/JobSystem.h"

// Sets default values
AGridWorldController::AGridWorldController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	World = CreateDefaultSubobject<UGridWorld>(TEXT("GridWorld"));
	AddOwnedComponent(World);
	TileManager = CreateDefaultSubobject<UTileManagerComponent>(TEXT("TileManager"));
	AddOwnedComponent(TileManager);
	WallManager = CreateDefaultSubobject<UWallManagerComponent>(TEXT("WallManager"));
	AddOwnedComponent(WallManager);
	
	WorldRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WorldRootComponent"));
	RootComponent = WorldRootComponent;
	FloorsRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FloorsRootComponent"));
	FloorsRootComponent->SetupAttachment(WorldRootComponent);
	WallsRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WallsRootComponent"));
	WallsRootComponent->SetupAttachment(WorldRootComponent);
}

UJobSystem* AGridWorldController::GetJobSystem() const
{
	return JobSystem;
}

UGridWorld* AGridWorldController::GetGridWorld() const
{
	return World;
}

void AGridWorldController::SetGridWorld(UGridWorld* const InWorld)
{
	this->World = InWorld;
}

UTileManagerComponent* AGridWorldController::GetTileManager() const
{
	return TileManager;
}

UWallManagerComponent* AGridWorldController::GetWallManager() const
{
	return WallManager;
}

// Called when the game starts or when spawned
void AGridWorldController::BeginPlay()
{
	Super::BeginPlay();
	ATopDownController* Controller = Cast<ATopDownController>(GetWorld()->GetFirstPlayerController());
	if (Controller)
	{
		Controller->SetWorldController(this);
	}
	JobSystem = NewObject<UJobSystem>();	
}

void AGridWorldController::OnInstalledObjectLoaded()
{
	//LogOnScreen(this, "Finished loading.", FColor::Green);

	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		TArray<UObject*> InstalledObjectsList;
		Manager->GetPrimaryAssetObjectList(FPrimaryAssetType("Installed Object"), InstalledObjectsList);
		for (auto Object : InstalledObjectsList)
		{
			UInstalledObject* InstalledObject = nullptr;
			if (IsValid(InstalledObject = Cast<UInstalledObject>(Object)))
			{
				Installables.Add(InstalledObject->ObjectType, InstalledObject);
				UE_LOG(LogTDTD_CPP, Display, TEXT("Added Installable: %s"), *InstalledObject->ObjectType.ToString());
			}
		}
	}
}

void AGridWorldController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	if(HasAuthority())
	{
		TArray<FPrimaryAssetId> InstalledObjectsIdList;
		UAssetManager* Manager = UAssetManager::GetIfValid();
		Manager->GetPrimaryAssetIdList(FPrimaryAssetType("Installed Object"), InstalledObjectsIdList);
		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &AGridWorldController::OnInstalledObjectLoaded);
		TArray<FName> Bundles;

		Manager->LoadPrimaryAssets(InstalledObjectsIdList, Bundles, Delegate);
	}
}

void AGridWorldController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	TileManager->InitFloorComponents(FloorsRootComponent);
	WallManager->InitWallComponents(WallsRootComponent);
	World->Init(ETileType::Ground);
	ClearAllInstances();
	InitInstances();
}

void AGridWorldController::InitInstances() const
{
	FTransform TileTransform;
	TileTransform.SetRotation(FQuat::Identity);
	for (int z = 0; z < World->Depth; ++z)
	{
		for (int x = 0; x < World->Size().X; ++x)
		{
			for (int y = 0; y < World->Size().Y; ++y)
			{
				UTile* Tile = World->GetTileAt(x, y, z);
				TileTransform.SetLocation(FVector(x, y, z) * World->TileSize());
				TileManager->InitTileInstance(TileTransform, Tile);
				WallManager->InitWallInstance(Tile);
			}
		}
	}
	FOnTileChanged TileChanged = FOnTileChanged::CreateUObject(TileManager, &UTileManagerComponent::OnTileChanged);
	World->RegisterTileChanged(TileChanged);
}

void AGridWorldController::ClearAllInstances() const
{
	TileManager->ClearTileInstances();
	WallManager->ClearWallInstances();
}

void AGridWorldController::InstallWallToTile(UTile* TileAt, const FName InstalledObjectName, bool Remove)
{
	FName NameToCheck;
	//no tile means no wall
	if (!IsValid(TileAt))
		return;
	if (IsValid(TileAt->InstalledObject))
	{
		//existing object, check it
		if (TileAt->InstalledObject->ObjectType != InstalledObjectName || InstalledObjectName == FName("Empty"))
		{
			// different, for now change to it (especially if new type should be "empty")
			NameToCheck = TileAt->InstalledObject->ObjectType;
			Remove = true;
		}
		else
		{
			// same type, bail
			return;
		}
	}
	else
	{
		// should be a new installed object here
		NameToCheck = InstalledObjectName;
	}
	if (Remove || InstalledObjectName == FName("Empty"))
	{
		TileAt->PlaceObject(nullptr);
	}
	else
	{
		// incorrect usage of UInstalledObject
		if (const UInstalledObject* Obj = *Installables.Find(NameToCheck))
		{
			UInstalledObject::PlaceInstance(Obj,TileAt);
		}
	}
	WallManager->PlaceWall(TileAt, NameToCheck, Remove);
}

UTile* AGridWorldController::GetTileAtWorldPos(const FVector& Loc) const
{
	return GetGridWorld()->GetTileAtWorldPos(Loc - GetActorLocation());
}

TArray<UTile*> AGridWorldController::GetNeighborTiles(const FVector IndexPos, const int Range) const
{
	return GetGridWorld()->GetNeighborTiles(IndexPos, Range);
}

void AGridWorldController::PlaceGhost(UTile* InTile, const FName& InstalledObjectName)
{
	if (InstalledObjectName.ToString().Contains("Wall"))
	{
		GetWallManager()->PlaceGhostWall(InTile, InstalledObjectName);
	}
}

FVector AGridWorldController::GetWorldPosForTileCenter(const FIntPoint& InPoint) const
{
	FVector Vector = FVector(InPoint);
	return Vector*GetGridWorld()->TileSize() + GetActorLocation();
}

UTile* AGridWorldController::GetTileAtWorldPos(const FVector InPos, FIntPoint& OutPoint)
{
	UTile* TileAt = GetTileAtWorldPos(InPos);
	OutPoint.X = TileAt->X();
	OutPoint.Y = TileAt->Y();
	return TileAt;
}

void AGridWorldController::TileClicked(UTile* Tile, ETileType NewType) const
{
	if (!Tile)
	{
		return;
	}
	if (Tile->GetType() != NewType)
	{
		UpdateTile(NewType, Tile);
	}
}

void AGridWorldController::TileRotate(UTile* Tile) const
{
	if (!Tile)
	{
		return;
	}
	Tile->SetRot(Tile->GetRot()+FRotator(0,90,0));
	UpdateTile(Tile->GetType(), Tile);
}

UTile* AGridWorldController::UpdateTile(const ETileType NewType, UTile* Tile) const
{
	if (Tile)
	{
		//turn into a callback
		Tile->SetType(NewType);
		TileManager->OnTileChanged(Tile);
	}
	return Tile;
}

// Called every frame
// ReSharper disable once CppParameterMayBeConst
void AGridWorldController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DrawTileDebug(GetWorld(), GetGridWorld(), GetActorLocation());
}
