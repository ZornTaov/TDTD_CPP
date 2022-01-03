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
				ETileDirection::Up,
				ETileDirection::Right,
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
				ETileDirection::Down,
				ETileDirection::Left,
				ETileDirection::DownLeft
			}
		}
	};
#pragma endregion SubMapMap

bool FWallStruct::IsConnected(TArray<FTile*>& Tiles, ETileDirection TileDirection)
{
	const FTile* ThisTile = Tiles[static_cast<int8>(ETileDirection::Center)];
	const FTile* OtherTile = Tiles[static_cast<uint8>(TileDirection)];
	return ThisTile->InstalledObject->ObjectType == OtherTile->InstalledObject->ObjectType;
}

TMap<EWallQuadrant, EWallSubTileType> FWallStruct::GetSubTileTypes(TArray<FTile*>& Tiles)
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
		case 5:
		case 1:
			OutArray.Add(Quadrant, Side);
		case 6:
		case 2:
			OutArray.Add(Quadrant, SideFlipped);
		case 7:
		case 3:
			OutArray.Add(Quadrant, InnerCorner);
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
		return FVector(-50,50,0);
	case EWallQuadrant::NorthEast: 
		return FVector(50,50,0);
	case EWallQuadrant::SouthEast: 
		return FVector(50,-50,0);
	case EWallQuadrant::SouthWest: 
		return FVector(-50,-50,0);
	default:
		checkNoEntry();
		return FVector();
	}
}
FRotator FWallStruct::GetQuadrantRotation(const EWallQuadrant Quadrant)
{
	switch (Quadrant) {
	case EWallQuadrant::NorthWest: 
		return FRotator(0,0,0);
	case EWallQuadrant::NorthEast: 
		return FRotator(0,90,0);
	case EWallQuadrant::SouthEast: 
		return FRotator(0,180,0);
	case EWallQuadrant::SouthWest: 
		return FRotator(0,270,0);
	default:
		checkNoEntry();
		return FRotator();
	}
}
