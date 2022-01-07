#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Tile.h"
#include "WallStruct.generated.h"

class UGridWorld;

UENUM(BlueprintType)
enum class ETileDirection : uint8
{
	UpLeft = 0,
	Up = 1,
	UpRight = 2,
	Left = 3,
	Center = 4,
	Right = 5,
	DownLeft = 6,
	Down = 7,
	DownRight = 8
};
UENUM(BlueprintType)
enum class EWallQuadrant : uint8
{
	NorthWest = 0,
	NorthEast = 1,
	SouthEast = 2,
	SouthWest = 3
};
ENUM_RANGE_BY_COUNT(EWallQuadrant, 4);

UENUM()
enum EWallSubTileType
{
	Fill,
	InnerCorner,
	Side,
	OuterCorner,
	SideFlipped
};

USTRUCT(BlueprintType)
struct FWallStruct : public FTableRowBase
{
	GENERATED_BODY()

	static TMap<EWallQuadrant,TArray<ETileDirection>> SubTileMap;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMesh* FillMesh = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMesh* InnerCornerMesh = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMesh* MiddleMesh = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMesh* OuterCornerMesh = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture* Icon = nullptr;

	static bool IsConnected(TArray<FTile*>& Tiles, ETileDirection TileDirection);
	static TMap<EWallQuadrant, EWallSubTileType> GetSubTileTypes(TArray<FTile*>& Tiles);
	static FVector GetQuadrantOffset(EWallQuadrant Quadrant);
	static FRotator GetQuadrantRotation(EWallQuadrant Quadrant);
};
