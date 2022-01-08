#include "WallStruct.h"

#include "GridWorld.h"
#include "InstalledObject.h"

TMap<EWallQuadrant, TArray<ETileDirection>> FWallStruct::SubTileMap =
#pragma region SubMapMap
	TMap<EWallQuadrant, TArray<ETileDirection>>{
		{
			EWallQuadrant::NorthWest, TArray<ETileDirection>{
				ETileDirection::Up,
				ETileDirection::Left,
				ETileDirection::UpLeft
			}
		},
		{
			EWallQuadrant::NorthEast, TArray<ETileDirection>{
				ETileDirection::Right,
				ETileDirection::Up,
				ETileDirection::UpRight
			}
		},
		{
			EWallQuadrant::SouthEast, TArray<ETileDirection>{
				ETileDirection::Down,
				ETileDirection::Right,
				ETileDirection::DownRight
			}
		},
		{
			EWallQuadrant::SouthWest, TArray<ETileDirection>{
				ETileDirection::Left,
				ETileDirection::Down,
				ETileDirection::DownLeft
			}
		}
	};
#pragma endregion SubMapMap

bool FWallStruct::IsConnected(TArray<UTile*>& Tiles, ETileDirection TileDirection)
{
	const UTile* ThisTile = Tiles[static_cast<int8>(ETileDirection::Center)];
	const UTile* OtherTile = Tiles[static_cast<uint8>(TileDirection)];
	if (!ThisTile || !OtherTile) return false;
	if (!ThisTile->InstalledObject || !OtherTile->InstalledObject) return false;
	if (ThisTile->InstalledObject->ObjectType == "" || OtherTile->InstalledObject->ObjectType == "") return false;
	if (ThisTile->InstalledObject->ObjectType == "Empty" || OtherTile->InstalledObject->ObjectType == "Empty") return false;

	return ThisTile->InstalledObject->ObjectType == OtherTile->InstalledObject->ObjectType;
}

TMap<EWallQuadrant, EWallSubTileType> FWallStruct::GetSubTileTypes(TArray<UTile*>& Tiles)
{

	TMap<EWallQuadrant, EWallSubTileType> OutArray;
	for (const EWallQuadrant Quadrant : TEnumRange<EWallQuadrant>())
	{
		TArray<ETileDirection> Neighbors = SubTileMap[Quadrant];
		uint8 Data = IsConnected(Tiles, Neighbors[0]);
		Data |= IsConnected(Tiles, Neighbors[1]) << 1;
		Data |= IsConnected(Tiles, Neighbors[2]) << 2;
		switch (Data)
		{
		case 0:
		case 4:
			OutArray.Add(Quadrant, OuterCorner);
			break;
		case 5:
		case 1:
			OutArray.Add(Quadrant, Side);
			break;
		case 6:
		case 2:
			OutArray.Add(Quadrant, SideFlipped);
			break;
		case 3:
			OutArray.Add(Quadrant, InnerCorner);
			break;
		case 7:
		default:
			OutArray.Add(Quadrant, Fill);
		}
	}
	return OutArray;
}

FVector FWallStruct::GetQuadrantOffset(const EWallQuadrant Quadrant)
{
	switch (Quadrant) {
	case EWallQuadrant::NorthWest: 
		return FVector(-50,-50,100);
	case EWallQuadrant::NorthEast: 
		return FVector(50,-50,100);
	case EWallQuadrant::SouthEast: 
		return FVector(50,50,100);
	case EWallQuadrant::SouthWest: 
		return FVector(-50,50,100);
	default:
		checkNoEntry();
		return FVector();
	}
}
FRotator FWallStruct::GetQuadrantRotation(const EWallQuadrant Quadrant)
{
	switch (Quadrant) {
	case EWallQuadrant::NorthWest: 
		return FRotator(0,180,0);
	case EWallQuadrant::NorthEast: 
		return FRotator(0,270,0);
	case EWallQuadrant::SouthEast: 
		return FRotator(0,0,0);
	case EWallQuadrant::SouthWest: 
		return FRotator(0,90,0);
	default:
		checkNoEntry();
		return FRotator();
	}
}
