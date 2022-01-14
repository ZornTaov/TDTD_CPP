// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorld/SelectionDecalActor.h"

#include "Tile.h"
#include "Engine/Canvas.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetRenderingLibrary.h"

ASelectionDecalActor::ASelectionDecalActor()
{
	this->GetDecal()->DecalSize = FVector(100.0f);

	static ConstructorHelpers::FObjectFinder<UCanvasRenderTarget2D> RenderTarget(
		TEXT("CanvasRenderTarget2D'/Game/Materials/SelectionRenderTarget.SelectionRenderTarget'")
		);
	if (RenderTarget.Succeeded())
	{
		SelectionRenderTarget = RenderTarget.Object;
	}
	SelectionRenderTarget->InitAutoFormat(100, 100);

	this->AActor::SetActorHiddenInGame(true);
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("SelectionBox"));
	Box->SetBoxExtent(FVector(100.0f));
	Box->SetupAttachment(GetDecal());
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

/*void ASelectionDecalActor::DrawToCanvasRenderTarget(UCanvas* Canvas, int32 Width, int32 Height)
{
	
}*/

void ASelectionDecalActor::OnSelectionChanged(TArray<UTile*>& Tiles, TFunction<FLinearColor(UTile*)>Pred)
{
	UCanvas * canvas;
	FVector2D canvasSize;
	FDrawToRenderTargetContext context;
 
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, SelectionRenderTarget, canvas, canvasSize, context);
	canvas->K2_DrawTexture(nullptr, FVector2D(0,0), canvasSize, FVector2D(0),
		FVector2D(1), FLinearColor::Black);
	for (auto Tile : Tiles)
	{
		if(!Tile) continue;
		FVector2D Pos = FVector2D(Tile->GetIndexPos());
		FVector2D Size = FVector2D(1, 1);
		canvas->K2_DrawTexture(nullptr, Pos, Size, FVector2D(0),
		FVector2D(1), Pred(Tile));
		/*if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow,
				FString::Printf(TEXT("Drawn at: %s"), *Pos.ToString()));*/
	}
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, context);
	/*if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow,
			FString::Printf(TEXT("Canvas Updated with %d Tiles."), Tiles.Num()));*/
}
