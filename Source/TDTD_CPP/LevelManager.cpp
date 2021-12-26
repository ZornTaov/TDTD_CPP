// Fill out your copyright notice in the Description page of Project Settings.
// Gotten from jchildren/LevelManager.cpp on github

//#include "LevelGeneration.h"
#include "LevelManager.h"

#include "Engine/DataTable.h"
#include "GridWorld/Tile.h"


// Sets default values
ALevelManager::ALevelManager() :
	TileSize(400),
	LevelHeight(400),
	FloorThickness(10),
	RandomSeed(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Levels.SetNum(1);
	ResizeLevels();
	
	RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	MeshRootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("MeshRootComponent"));
	FloorsRootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("FloorsRootComponent"));
	WallsRootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("WallsRootComponent"));

	MeshRootComponent->SetupAttachment(RootComponent);
	FloorsRootComponent->SetupAttachment(MeshRootComponent);
	WallsRootComponent->SetupAttachment(MeshRootComponent);

	
	static ConstructorHelpers::FObjectFinder<UDataTable> Tdt(TEXT("DataTable'/Game/Blueprints/TileInfoTable.TileInfoTable'"));
	if (Tdt.Succeeded())
	{
		TileDataTable = Tdt.Object;
	}
	
	//InitAllComponents();
	InitInstances();
}

void ALevelManager::BeginDestroy()
{
	Super::BeginDestroy();
	ClearAllInstances();
	ClearAllComponents();
}

// Called when the game starts or when spawned
void ALevelManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
/*void ALevelManager::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}*/

void ALevelManager::ClearAllComponents()
{
	ClearComponents(FloorComponents);
	ClearComponents(WallComponents);
}

void ALevelManager::ClearComponents(TArray<UInstancedStaticMeshComponent*> &Components)
{
	UnregisterComponents(Components);
	Components.Empty();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ALevelManager::UnregisterComponents(TArray<UInstancedStaticMeshComponent*> &Components)
{
	for (UInstancedStaticMeshComponent*& Component : Components)
	{
		if(Component== nullptr) continue;
		Component->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepRelative, false));
		Component->DestroyComponent();
	}
}

void ALevelManager::InitAllComponents()
{
	InitComponents(FloorComponents,
		TEXT("Floor_Component_"), FloorsRootComponent);

	//InitComponents(WallComponents, WallTypes,
	//	TEXT("Wall_Component_"), WallsRootComponent);
}

void ALevelManager::InitComponents(TArray<UInstancedStaticMeshComponent*> &Components,
                                   const FString BaseComponentName, USphereComponent* ParentComp)
{
	TArray<FTileStruct*> OutRowArray;
	TileDataTable->GetAllRows<FTileStruct>(TEXT(""),OutRowArray);
	if(!ensure(OutRowArray.Num() > 0)) return;
	Components.SetNum(TileDataTable->GetRowNames().Num());
	for (auto Component(Components.CreateIterator()); Component; ++Component)
	{
		FString ComponentName = BaseComponentName;
		ComponentName.AppendInt(Component.GetIndex());
		*Component = AddInstancedStaticMeshComponent(*OutRowArray[Component.GetIndex()], ComponentName, ParentComp);
	}
}

void ALevelManager::AddComponents(TArray<UInstancedStaticMeshComponent*> &Components, const int32 Count,
                                  const FString BaseComponentName, USphereComponent* ParentComp)
{
	TArray<FTileStruct*> OutRowArray;
	TileDataTable->GetAllRows(TEXT(""),OutRowArray);
	for (int32 i = Components.Num(); i < Count; ++i)
	{
		FString ComponentName = BaseComponentName;
		ComponentName.AppendInt(i - 1);
		Components.Add(AddInstancedStaticMeshComponent(*OutRowArray[i-1], ComponentName, ParentComp));
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ALevelManager::PopComponents(TArray<UInstancedStaticMeshComponent*> &Components, const int32 Count)
{
	for (int32 i = Components.Num(); i > Count; --i)
	{
		Components[i - 1]->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepRelative, false));
		Components[i - 1]->DestroyComponent();
		Components.Pop();
	}
}

UInstancedStaticMeshComponent* ALevelManager::AddInstancedStaticMeshComponent(FTileStruct Tile, const FString ComponentName, USphereComponent* ParentComp)
{
	
	UInstancedStaticMeshComponent* ComponentPtr;
	if (FUObjectThreadContext::Get().IsInConstructor)
	{
		ComponentPtr = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName(*ComponentName));
		ComponentPtr->SetupAttachment(ParentComp);
		ComponentPtr->SetStaticMesh(Tile.Mesh);
	}
	else
	{
		ComponentPtr = NewObject<UInstancedStaticMeshComponent>(this, FName(*ComponentName));
		ComponentPtr->AttachToComponent(ParentComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		ComponentPtr->RegisterComponent();
	}
	return ComponentPtr;
}

void ALevelManager::UpdateLevel()
{
	//ResizeLevels();
	//InitAllComponents();
	//ClearAllInstances();
	//InitInstances();
	
}

void ALevelManager::InitInstances()
{

	// We use iterators rather than ranged based for loops
	// here as we need the index of the TArray to determine
	// the location of each static mesh instance
	for (auto Level(Levels.CreateIterator()); Level; ++Level)
	{
		FTransform Transform;
		Transform.SetRotation(FQuat(FRotator(0.0f, 0.0f, 0.0f)));
		const int32 z = (LevelHeight + FloorThickness) * Level.GetIndex();

		for (auto Floor(Level->Floors.CreateIterator()); Floor; ++Floor)
		{
			const int Type = *Floor;
			switch (Type)
			{
			case 0:
				break;
			default:
				const int32 x = (Floor.GetIndex() / MaxY) * TileSize;
				const int32 y = (Floor.GetIndex() % MaxY) * TileSize;

				Transform.SetLocation(FVector(x, y, z));

				// We subtract 1 as Floor == 0 is empty
				if (FloorComponents.Num() >= Type)
				{
					FloorComponents[Type - 1]->AddInstance(Transform);
				}

				break;
			}
		}

		for (auto XWall(Level->XWalls.CreateIterator()); XWall; ++XWall)
		{

			switch (*XWall)
			{
			case 0:
				break;
			default:
				const int32 x = ((XWall.GetIndex() / 2) / (MaxY + 1)) * TileSize;
				const int32 y = ((XWall.GetIndex() / 2) % (MaxY + 1)) * TileSize;

				Transform.SetLocation(FVector(x, y, z));

				// We subtract 1 as Wall == 0 is void
				
				if (WallComponents.Num() >= *XWall)
				{
					WallComponents[*XWall - 1]->AddInstance(Transform);
				}
				break;
			}
		}

		Transform.SetRotation(FQuat(FRotator(0.0f, 90.0f, 0.0f)));

		for (auto YWall(Level->YWalls.CreateIterator()); YWall; ++YWall)
		{

			switch (*YWall)
			{
			case 0:
				break;
			default:
				const int32 x = ((YWall.GetIndex() / 2) / MaxY) * TileSize;
				const int32 y = ((YWall.GetIndex() / 2) % MaxY) * TileSize;

				Transform.SetLocation(FVector(x, y, z));

				// We subtract 1 as Wall == 0 is void
				if (WallComponents.Num() >= *YWall)
				{
					WallComponents[*YWall - 1]->AddInstance(Transform);
				}
				break;
			}
		}
	}
}

void ALevelManager::ClearAllInstances()
{
	ClearInstances(FloorComponents);
	ClearInstances(WallComponents);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ALevelManager::ClearInstances(TArray<UInstancedStaticMeshComponent*> &Components)
{
	for (UInstancedStaticMeshComponent*& Component : Components)
	{
		if(Component == nullptr) continue;
		Component->ClearInstances();
	}
}

void ALevelManager::ResizeLevels()
{
	for (FStructLevel& Level : Levels)
	{
		Level.Resize(MaxX, MaxY);
	}
}

void ALevelManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	/*ResizeLevels();
	ClearAllInstances();
	
	if (FloorTypes == 0)
	{
		ClearComponents(FloorComponents);
	}
	else if (FloorComponents.Num() == 0)
	{
		InitComponents(FloorComponents, FloorTypes,
			TEXT("Floor_Component_"), FloorsRootComponent);
	}
	else if (FloorTypes > FloorComponents.Num())
	{
		AddComponents(FloorComponents, FloorTypes,
			TEXT("Floor_Component_"), FloorsRootComponent);
	}
	else if (FloorTypes < FloorComponents.Num())
	{
		PopComponents(FloorComponents, FloorTypes);
	}

	if (WallTypes == 0)
	{
		ClearComponents(WallComponents);
	}
	else if (WallComponents.Num() == 0)
	{
		InitComponents(WallComponents, WallTypes,
			TEXT("Wall_Component_"), WallsRootComponent);
	}
	else if (WallTypes > WallComponents.Num())
	{
		AddComponents(WallComponents, WallTypes,
			TEXT("Wall_Component_"), WallsRootComponent);
	}
	else if (WallTypes < WallComponents.Num())
	{
		PopComponents(WallComponents, WallTypes);
	}
	InitInstances();*/
}
#if WITH_EDITOR
void ALevelManager::PostEditChangeProperty(FPropertyChangedEvent& E)
{
	const FName PropertyName = (E.Property != nullptr) ? E.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelManager, MaxX))
	{
		ResizeLevels();
		ClearAllInstances();
		InitInstances();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelManager, MaxY))
	{
		ResizeLevels();
		ClearAllInstances();
		InitInstances();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelManager, MaxZ))
	{
		Levels.SetNum(MaxZ);
		ResizeLevels();
		ClearAllInstances();
		InitInstances();
	}
	/*else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelManager, FloorTypes))
	{
		if (FloorTypes == 0)
		{
			ClearComponents(FloorComponents);
		}
		else if (FloorComponents.Num() == 0)
		{
			InitComponents(FloorComponents, FloorTypes,
				TEXT("Floor_Component_"), FloorsRootComponent);
		}
		else if (FloorTypes > FloorComponents.Num())
		{
			AddComponents(FloorComponents, FloorTypes,
				TEXT("Floor_Component_"), FloorsRootComponent);
		}
		else if (FloorTypes < FloorComponents.Num())
		{
			PopComponents(FloorComponents, FloorTypes);
		}
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelManager, WallTypes))
	{
		if (WallTypes == 0)
		{
			ClearComponents(WallComponents);
		}
		else if (WallComponents.Num() == 0)
		{
			InitComponents(WallComponents, WallTypes,
				TEXT("Wall_Component_"), WallsRootComponent);
		}
		else if (WallTypes > WallComponents.Num())
		{
			AddComponents(WallComponents, WallTypes,
				TEXT("Wall_Component_"), WallsRootComponent);
		}
		else if (WallTypes < WallComponents.Num())
		{
			PopComponents(WallComponents, WallTypes);
		}
	}*/
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ALevelManager, Levels))
	{
		ClearAllInstances();
		InitInstances();
	}

	Super::PostEditChangeProperty(E);
}

#endif