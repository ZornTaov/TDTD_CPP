#include "WorldGridGraph.h"

#include "GridWorld/Tile.h"

int32 WorldGridGraph::GetNeighbourCount(FNodeRef NodeRef) const
{
	return 8;
}

bool WorldGridGraph::IsValidRef(FNodeRef NodeRef) const
{
	return WorldGrid->GetTileAt(FVector(NodeRef)) != nullptr;
}

WorldGridGraph::FNodeRef WorldGridGraph::GetNeighbour(const FNodeRef NodeRef, const int32 NeighbourIndex) const
{
	UTile* NeighborTile = WorldGrid->GetNeighborTiles(FVector(NodeRef, 0))[NeighbourIndex];
	FVector IndexPos;
	if (IsValid(NeighborTile))
		IndexPos = NeighborTile->GetIndexPos();
	else
		return FNodeRef::NoneValue;
	return FNodeRef(IndexPos.X, IndexPos.Y);
}