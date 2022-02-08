#pragma once

#include "CoreMinimal.h"
#include "NavigationData.h"
#include "GraphAStar.h"
#include "WorldGridGraph.h"
#include "GridWorld/GridWorldController.h"
#include "GridWorldNavigationData.generated.h"

UCLASS(config = Engine, defaultconfig, hidecategories = (Input, Rendering, Tags, "Utilities|Transformation", Actor, Layers, Replication), notplaceable)
class TDTD_CPP_API AGridWorldNavigationData : public ANavigationData
{
	GENERATED_BODY()

	AGridWorldNavigationData(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

public:
	static FPathFindingResult FindPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query);

	bool ProjectPoint(const FVector& Point, FNavLocation& OutLocation, const FVector& Extent, FSharedConstNavQueryFilter Filter = NULL, const UObject* Querier = NULL) const override;

private:
	AGridWorldController* WorldGridActor;

	WorldGridGraph Graph;
	FGraphAStar<WorldGridGraph>* Pathfinder;
};