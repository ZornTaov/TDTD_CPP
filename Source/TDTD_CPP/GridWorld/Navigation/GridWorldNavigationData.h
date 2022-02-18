#pragma once

#include "CoreMinimal.h"
#include "NavigationData.h"
#include "GraphAStar.h"
#include "WorldGridGraph.h"
#include "GridWorld/GridWorldController.h"
#include "NavMesh/RecastNavMesh.h"
#include "GridWorldNavigationData.generated.h"

DECLARE_CYCLE_STAT(TEXT("GridWorld Pathfinding"), STAT_Navigation_GridWorldPathfinding, STATGROUP_Navigation)

UCLASS(config = Engine, defaultconfig, hidecategories = (Input, Rendering, Tags, "Utilities|Transformation", Actor, Layers, Replication), notplaceable)
class TDTD_CPP_API AGridWorldNavigationData : public ARecastNavMesh
{
	GENERATED_BODY()

	AGridWorldNavigationData(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

public:
	static FPathFindingResult FindPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query);
	//static bool TestPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query, int32* NumVisitedNodes);

	virtual bool ProjectPoint(const FVector& Point, FNavLocation& OutLocation, const FVector& Extent, FSharedConstNavQueryFilter Filter = NULL, const UObject* Querier = NULL) const override;

private:
	UPROPERTY()
	AGridWorldController* WorldGridActor;

	WorldGridGraph Graph;
	FGraphAStar<WorldGridGraph>* Pathfinder;
};