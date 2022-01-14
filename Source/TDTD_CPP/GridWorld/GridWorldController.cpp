// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldController.h"

#include "DrawDebugHelpers.h"
#include "GridWorld.h"
#include "InstalledObject.h"
#include "TDTDExtensionHelpers.h"
#include "VarDump.h"
#include "WallStruct.h"
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
	static ConstructorHelpers::FObjectFinder<UDataTable> Wdt(TEXT("DataTable'/Game/Blueprints/WallInfoTable.WallInfoTable'"));
	if (Wdt.Succeeded())
	{
		WallTileDataTable = Wdt.Object;
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
			FVector Location = FVector(x, y, Layer)*World->TileWidth;
			ATileActor* TileActor = AActor::GetWorld()->SpawnActor<ATileActor>(Location, FRotator::ZeroRotator, Params);
			//Tile_Data->Del.AddLambda([TileActor, this](FTile* Tile){OnTileChanged(*Tile, this->FloorTileDataTable, TileActor);});
			Tile_Data->SetType(ETileType::Floor);
		}
	}
	GetWorld()->GetNavigationSystem()->InitializeForWorld(*GetWorld(), FNavigationSystemRunMode::GameMode);*/
}

void AGridWorldController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	InitFloorComponents(FloorComponents, FloorsRootComponent, FloorTileDataTable);
	InitWallComponents(WallComponents, WallsRootComponent, WallTileDataTable);
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
				UTile* Tile = World->GetTileAt(x, y, z);
				TileTransform.SetLocation(FVector(x * World->TileWidth, y * World->TileWidth, z * World->TileThickness));
				const uint8 Type = static_cast<uint8>(Tile->GetType());
				if (Type && Type < FloorComponents.Num())
				{
					FloorComponents[Type]->AddInstance(TileTransform);
				}
				/*const uint8 WallType = static_cast<uint8>(Tile->GetWallType());
				if (WallType && WallType < WallComponents.Num())
				{
					Tile->InstanceIndex = WallComponents[WallType]->AddWall(TileTransform);
				}*/
			}
		}
	}
}

void AGridWorldController::InitFloorComponents(TArray<UInstancedStaticMeshComponent*>& Components, const USceneComponent* ParentComp, const UDataTable* Data) const
{
	if (ParentComp->GetNumChildrenComponents() != 0)
	{
		TArray<FTileStruct*> OutTileRowArray;
		Data->GetAllRows<FTileStruct>(TEXT("GridWorldController#InitFloorComponents"),OutTileRowArray);
		Components.SetNum(ParentComp->GetNumChildrenComponents());
		for (int i = 0; i < ParentComp->GetNumChildrenComponents(); ++i)
		{
			UInstancedStaticMeshComponent* Child = Cast<UInstancedStaticMeshComponent>(ParentComp->GetChildComponent(i));
			if(IsValid(Child))
			{
				Child->SetStaticMesh(OutTileRowArray[i]->Mesh);	
				Components[i] = Child;
			}
		}
	}
}

void AGridWorldController::InitWallComponents(TArray<UWallTypeComponent*>& Components, const USceneComponent* ParentComp, const UDataTable* Data) const
{
	if (ParentComp->GetNumChildrenComponents() != 0)
	{
		TArray<FWallStruct*> OutWallRowArray;
		Data->GetAllRows<FWallStruct>(TEXT("GridWorldController#InitWallComponents"),OutWallRowArray);
		if (OutWallRowArray.Num() == 0)
		{
			return;
		}
		Components.SetNum(ParentComp->GetNumChildrenComponents());
		for (int i = 0; i < ParentComp->GetNumChildrenComponents(); ++i)
		{
			UWallTypeComponent* Child = Cast<UWallTypeComponent>(ParentComp->GetChildComponent(i));
			if(IsValid(Child))
			{
				Child->FillISM->SetStaticMesh(OutWallRowArray[i]->FillMesh);
				Child->InnerCornerISM->SetStaticMesh(OutWallRowArray[i]->InnerCornerMesh);
				Child->MiddleISM->SetStaticMesh(OutWallRowArray[i]->MiddleMesh);
				Child->OuterCornerISM->SetStaticMesh(OutWallRowArray[i]->OuterCornerMesh);
				Child->WallTypeName = Data->GetRowNames()[i];
				Components[i] = Child;
			}
		}
	}
}

void AGridWorldController::ClearAllInstances()
{
	ClearTileInstances(FloorComponents);
	ClearWallInstances(WallComponents);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void AGridWorldController::ClearTileInstances(TArray<UInstancedStaticMeshComponent*>& Components)
{
	for (UInstancedStaticMeshComponent*& Component : Components)
	{
		if(Component == nullptr) continue;
		if(IsValid(Component))
			Component->ClearInstances();
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void AGridWorldController::ClearWallInstances(TArray<UWallTypeComponent*> Components)
{
	for (UWallTypeComponent*& Component : Components)
	{
		if(Component == nullptr) continue;
		if(IsValid(Component))
			Component->ClearInstances();
	}
}

void AGridWorldController::TileClicked(const FVector& Vector, ETileType NewType) const
{
	UTile* Tile = World->GetTileAtWorldPos(Vector);
	if (!Tile)
	{
		return;
	}
	if (Tile->GetType() != NewType)
	{
		UpdateTile(Tile->GetIndexPos(), NewType, Tile);
	}
}

void AGridWorldController::TileRotate(const FVector& Vector) const
{
	UTile* Tile = World->GetTileAtWorldPos(Vector);
	if (!Tile)
	{
		return;
	}
	Tile->SetRot(Tile->GetRot()+FRotator(0,90,0));
	UpdateTile(Tile->GetIndexPos(), Tile->GetType(), Tile);
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
	const UWallTypeComponent* WallPlacer = nullptr;
	UWallTypeComponent** Res = WallComponents.FindByPredicate([NameToCheck](const UWallTypeComponent* WallType){return WallType->WallTypeName == NameToCheck;});
	if (Res && *Res)
	{
		WallPlacer = *Res;
	}
	if (IsValid(WallPlacer))
	{
		
		if (Remove || InstalledObjectName == FName("Empty"))
		{
			TileAt->PlaceObject(nullptr);
			WallPlacer->RemoveWall(GetGridWorld(),TileAt->GetIndexPos());
		}
		else
		{
			// incorrect usage of UInstalledObject
			UInstalledObject* Proto = UInstalledObject::CreatePrototype(NameToCheck);
			UInstalledObject::PlaceInstance(Proto,TileAt);
			WallPlacer->AddWall(GetGridWorld(), TileAt->GetIndexPos());
		}
	}
}

void AGridWorldController::GetIndex(const UTile* TileData, const uint8 OldTypeIndex, int& Index) const
{
	for (int i = 0; i < FloorComponents[OldTypeIndex]->GetInstanceCount(); ++i)
	{
		FTransform Transform;
		FloorComponents[OldTypeIndex]->GetInstanceTransform(i, Transform);
		if (Transform.GetLocation().Equals(TileData->GetWorldPos()))
		{
			Index = i;
			break;
		}
	}
}

void AGridWorldController::OnTileTypeChanged(const UTile* TileData, ETileType NewType) const
{
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
	GetIndex(TileData, OldTypeIndex, Index);
	//Remove from old Type
	if(FloorComponents[OldTypeIndex]->InstanceBodies.IsValidIndex(Index))
	{
		// TODO: Temporary fix for LogNavigationDirtyArea warnings. but not proper solution....
		//FloorComponents[OldTypeIndex]->bNavigationRelevant = FloorComponents[OldTypeIndex]->bNavigationRelevant && FloorComponents[OldTypeIndex]->InstanceBodies.Num() > 1;
		FloorComponents[OldTypeIndex]->RemoveInstance(Index);
	}
	//Add to new Type
	FloorComponents[NewTypeIndex]->AddInstance(FTransform(TileData->GetRot().Quaternion(), TileData->GetWorldPos()));
}

UTile* AGridWorldController::UpdateTile(const FVector Pos, const ETileType& NewType, UTile* Tile) const
{
	return UpdateTile(Pos.X, Pos.Y, Pos.Z, NewType, Tile);
}

UTile* AGridWorldController::UpdateTile(const int X, const int Y, const int Z, const ETileType NewType, UTile* Tile) const
{
	if (!Tile)
	{
		Tile = World->GetTileAt(X, Y, Z);
	}
	//turn into a callback
	OnTileTypeChanged(Tile, NewType);
	Tile->SetType(NewType);
	return Tile;
}

// Called every frame
// ReSharper disable once CppParameterMayBeConst
void AGridWorldController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DrawTileDebug();
}

void AGridWorldController::DrawDebug(const FVector Pos, const FString Str) const
{
	const FColor DrawColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f).ToFColor(true);
	DrawDebugString(GetWorld(), Pos, *Str, nullptr, DrawColor, 0.0f, true);
}

void AGridWorldController::DrawTileDebug() const
{
	for (int x = 0; x < World->Width; ++x)
	{
		for (int y = 0; y < World->Height; ++y)
		{
			for (int z = 0; z < World->Depth; ++z)
			{
				FVector Pos = FVector(x,y,z);
				const UTile* Tile = World->GetTileAt(Pos);
				DrawDebug(Tile->GetWorldPos() + FVector(0, 0, 100) + GetActorLocation(), FString::Printf(
							  TEXT("Index:%s\nTileType:%s\nInstalledType:%s"), *Pos.ToCompactString(),
							  *GetEnumName(Tile->GetType()),
							  *(Tile->InstalledObject ? Tile->InstalledObject->ObjectType.ToString() : TEXT(""))));
			}
		}		
	}
}
