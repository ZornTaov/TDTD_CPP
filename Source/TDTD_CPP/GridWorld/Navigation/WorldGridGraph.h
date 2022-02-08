#pragma once

#include "CoreMinimal.h"
#include "GraphAStar.h"
#include "GridWorld/GridWorld.h"

class TDTD_CPP_API WorldGridGraphQueryFilter
{
public:
    WorldGridGraphQueryFilter()
        : WorldGrid(nullptr)
        , AllowPartialPaths(false)
    {}

    void SetWorldGrid(UGridWorld* InWorldGrid)
    {
        WorldGrid = InWorldGrid;
    }

    float GetHeuristicScale() const
    {
        return 1;
    }

    float GetHeuristicCost(const FIntPoint StartNodeRef, const FIntPoint EndNodeRef) const
    {
        return FMath::Abs(StartNodeRef.X - EndNodeRef.X) + FMath::Abs(StartNodeRef.Y - EndNodeRef.Y);
    }

    float GetTraversalCost(const FIntPoint StartNodeRef, const FIntPoint EndNodeRef) const
    {
        return 1;
    }

    bool IsTraversalAllowed(const FIntPoint NodeA, const FIntPoint NodeB) const
    {
        if (!WorldGrid->IsLocationWalkable(NodeA) || !WorldGrid->IsLocationWalkable(NodeB))
            return false;

        static const FIntPoint kXOffset(1, 0);
        static const FIntPoint kYOffset(0, 1);

        if (NodeB.X < NodeA.X)
        {
            if (NodeB.Y < NodeA.Y)
            {
                if (!WorldGrid->IsLocationWalkable(NodeA - kXOffset) || !WorldGrid->IsLocationWalkable(NodeA - kYOffset))
                    return false;
            }
            else if (NodeB.Y > NodeA.Y)
            {
                if (!WorldGrid->IsLocationWalkable(NodeA - kXOffset) || !WorldGrid->IsLocationWalkable(NodeA + kYOffset))
                    return false;
            }
        }
        else if (NodeB.X > NodeA.X)
        {
            if (NodeB.Y < NodeA.Y)
            {
                if (!WorldGrid->IsLocationWalkable(NodeA + kXOffset) || !WorldGrid->IsLocationWalkable(NodeA - kYOffset))
                    return false;
            }
            else if (NodeB.Y > NodeA.Y)
            {
                if (!WorldGrid->IsLocationWalkable(NodeA + kXOffset) || !WorldGrid->IsLocationWalkable(NodeA + kYOffset))
                    return false;
            }
        }
    
        return true;
    }

    bool WantsPartialSolution() const
    {
        return AllowPartialPaths;
    }

    void SetAllowPartialPaths(bool allow)
    {
        AllowPartialPaths = allow;
    }

private:
    UGridWorld* WorldGrid;
    bool AllowPartialPaths;
};

class TDTD_CPP_API WorldGridGraph
{
public:
    void SetWorldGrid(UGridWorld* InWorldGrid)
    {
        WorldGrid = InWorldGrid;
    }

    typedef FIntPoint FNodeRef;

    int32 GetNeighbourCount(FNodeRef NodeRef) const;
    bool IsValidRef(FNodeRef NodeRef) const;
    FNodeRef GetNeighbour(const FNodeRef NodeRef, const int32 NeighbourIndex) const;

private:
    UGridWorld* WorldGrid;
};