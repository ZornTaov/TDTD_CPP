#include "GridWorldNavigationData.h"
#include "AI/Navigation/NavigationTypes.h"
#include "EngineUtils.h"
#include "NavigationData.h"
#include "WorldGridGraph.h"

AGridWorldNavigationData::AGridWorldNavigationData(const FObjectInitializer& ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		FindPathImplementation = FindPath;
		FindHierarchicalPathImplementation = FindPath;

		Pathfinder = new FGraphAStar<WorldGridGraph>(Graph);
	}
}

void AGridWorldNavigationData::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AGridWorldController> It(GetWorld(), AGridWorldController::StaticClass()); It; ++It)
	{
		WorldGridActor = *It;
		Graph.SetWorldGrid(WorldGridActor->GetGridWorld());
		break;
	}
}

bool AGridWorldNavigationData::ProjectPoint(const FVector& Point, FNavLocation& OutLocation, const FVector& Extent, FSharedConstNavQueryFilter Filter, const UObject* Querier) const
{
	if (WorldGridActor->GetTileAtWorldPos(Point))
	{
		OutLocation = FNavLocation(Point);
		return true;
	}

	return false;
}

FPathFindingResult AGridWorldNavigationData::FindPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query)
{
	const ANavigationData* Self = Query.NavData.Get();
	const AGridWorldNavigationData* AStar = Cast<const AGridWorldNavigationData>(Self);
	check(AStar != nullptr);

	if (AStar == nullptr)
		return ENavigationQueryResult::Error;

	FPathFindingResult Result(ENavigationQueryResult::Error);
	Result.Path = Query.PathInstanceToFill.IsValid() ? Query.PathInstanceToFill : Self->CreatePathInstance<FNavigationPath>(Query);

	FNavigationPath* NavPath = Result.Path.Get();

	if (NavPath != nullptr)
	{
		if ((Query.StartLocation - Query.EndLocation).IsNearlyZero())
		{
			Result.Path->GetPathPoints().Reset();
			Result.Path->GetPathPoints().Add(FNavPathPoint(Query.EndLocation));
			Result.Result = ENavigationQueryResult::Success;
		}
		else if (Query.QueryFilter.IsValid())
		{
			FIntPoint myGridPos, targetGridPos;
			AStar->WorldGridActor->GetTileAtWorldPos(Query.StartLocation.GridSnap(AStar->WorldGridActor->GetGridWorld()->TileWidth), myGridPos);
			AStar->WorldGridActor->GetTileAtWorldPos(Query.EndLocation, targetGridPos);

			WorldGridGraphQueryFilter queryFilter;
			queryFilter.SetWorldGrid(AStar->WorldGridActor->GetGridWorld());
			queryFilter.SetAllowPartialPaths(Query.bAllowPartialPaths);

			TArray<FIntPoint> Path;
			EGraphAStarResult aStarResult = AStar->Pathfinder->FindPath(myGridPos, targetGridPos, queryFilter, Path);

			if (aStarResult == EGraphAStarResult::SearchFail || aStarResult == EGraphAStarResult::InfiniteLoop)
			{
				UE_LOG(LogTemp, Warning, TEXT("AWorldGridNavigationData A* fail: %d"), aStarResult);

				Result.Result = ENavigationQueryResult::Fail;
				return Result;
			}

			Path.Insert(myGridPos, 0);

			for (auto& point : Path)
			{
				NavPath->GetPathPoints().Add(FNavPathPoint(AStar->WorldGridActor->GetWorldPosForTileCenter(point)));
			}

			UE_LOG(LogTemp, Warning, TEXT("WorldGridNav path (%d points):"), Path.Num());
			for (int i = 0; i < Path.Num(); i++)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s (%s)"), *Path[i].ToString(), *NavPath->GetPathPoints()[i].Location.ToString());
			}
			UE_LOG(LogTemp, Warning, TEXT("WorldGridNav path end"));

			NavPath->MarkReady();
			Result.Result = ENavigationQueryResult::Success;
		}
	}

	return Result;
}