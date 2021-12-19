// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum ETileType { Empty, Ground, Floor };

/**
 * 
 */
class TDTD_CPP_API FTile
{
public:
	ETileType Type = Empty;
	
	FVector Pos;
	FTile();
	~FTile();
};

