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
	const FVector IndexPos = WorldGrid->GetNeighborTiles(FVector(NodeRef, 0))[NeighbourIndex]->GetIndexPos();
	return FNodeRef(IndexPos.X, IndexPos.Y);
}