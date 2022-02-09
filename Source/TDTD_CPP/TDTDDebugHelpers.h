#pragma once

static void DrawDebug(const UWorld* InWorld, const FVector Pos, const FString Str)
{
	const FColor DrawColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f).ToFColor(true);
	DrawDebugString(InWorld, Pos, *Str, nullptr, DrawColor, 0.0f, true);
}

static void DrawTileDebug(const UWorld* InWorld, UGridWorld* GridWorld, const FVector Pos)
{
	FHitResult Hit;
	InWorld->GetGameInstance()->GetPrimaryPlayerController()->GetHitResultUnderCursor(ECC_Camera, false, Hit);
	FVector CursorL = Hit.ImpactPoint.GridSnap(GridWorld->TileWidth);
	CursorL.Z = Hit.ImpactPoint.Z;
	TArray<UTile*> Tiles = GridWorld->GetNeighborTiles(
		(CursorL - Pos)/GridWorld->TileSize(),
		2
		);
	for (const UTile* Tile : Tiles)
	{
		if (Tile)
			DrawDebug(InWorld, Tile->GetWorldPos() + FVector(0, 0, 100), FString::Printf(
					  TEXT("Index:%d,%d\nTile:%s\nInstalled:%s\nJob:%s"), static_cast<int>(Tile->GetIndexPos().X),static_cast<int>(Tile->GetIndexPos().Y),
					  *GetEnumName(Tile->GetType()),
					  *(Tile->InstalledObject ? Tile->InstalledObject->ObjectType.ToString() : TEXT("Empty")),
					  *(Tile->PendingJobs.IsValidIndex(0) ? Tile->PendingJobs[0]->JobName.ToString() : TEXT("None")))
					  );
	}
}