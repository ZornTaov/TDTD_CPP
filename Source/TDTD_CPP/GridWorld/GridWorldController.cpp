// Fill out your copyright notice in the Description page of Project Settings.

#include "GridWorldController.h"

#include "DrawDebugHelpers.h"
#include "GridWorld.h"
#include "Installables/InstalledObject.h"
#include "TDTDExtensionHelpers.h"
#include "TileManagerComponent.h"
#include "VarDump.h"
#include "TopDownController.h"
#include "WallManagerComponent.h"
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
		UInstalledObject* Proto = UInstalledObject::CreatePrototype(NameToCheck);
		UInstalledObject::PlaceInstance(Proto,TileAt);
	}
	WallManager->PlaceWall(TileAt, NameToCheck, Remove);
}

UTile* AGridWorldController::GetTileAtWorldPos(const FVector& Loc) const
{
	return GetGridWorld()->GetTileAtWorldPos(Loc - GetActorLocation());
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
		TileManager->OnTileChanged(Tile, NewType);
		Tile->SetType(NewType);
	}
	return Tile;
}

// Called every frame
// ReSharper disable once CppParameterMayBeConst
void AGridWorldController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawTileDebug();
}

void AGridWorldController::DrawDebug(const FVector Pos, const FString Str) const
{
	const FColor DrawColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f).ToFColor(true);
	DrawDebugString(GetWorld(), Pos, *Str, nullptr, DrawColor, 0.0f, true);
}

void AGridWorldController::DrawTileDebug() const
{
	FHitResult Hit;
	GetGameInstance()->GetPrimaryPlayerController()->GetHitResultUnderCursor(ECC_Camera, false, Hit);
	FVector CursorL = Hit.ImpactPoint.GridSnap(World->TileWidth);
	CursorL.Z = Hit.ImpactPoint.Z;
	TArray<UTile*> Tiles = GetGridWorld()->GetNeighborTiles(
		(CursorL - GetActorLocation())/GetGridWorld()->TileSize(),
		2
		);
	for (const UTile* Tile : Tiles)
	{
		if (Tile)
			DrawDebug(Tile->GetWorldPos() + FVector(0, 0, 100) + GetActorLocation(), FString::Printf(
					  TEXT("Index:%s\nTileType:%s\nInstalledType:%s"), *Tile->GetIndexPos().ToCompactString(),
					  *GetEnumName(Tile->GetType()),
					  *(Tile->InstalledObject ? Tile->InstalledObject->ObjectType.ToString() : TEXT("Empty"))));
	}
}
