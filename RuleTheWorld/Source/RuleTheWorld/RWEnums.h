// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

UENUM()
enum class ECharacterState : int8
{
	Live UMETA(DisplayName = "Live"),
	Dead UMETA(DisplayName = "Dead")
};

UENUM()
enum class EAnimalState : int8
{
	Live UMETA(DisplayName = "Live"),
	Dead UMETA(DisplayName = "Dead")
};

UENUM()
enum class EAnimalData : int8
{
	None UMETA(DisplayName = "None"),
	Wolf UMETA(DisplayName = "Wolf"),
	Pig UMETA(DisplayName = "Pig"),
	Fox UMETA(DisplayName = "Fox"),
};

UENUM()
enum class EItemData : int8
{
	None UMETA(DisplayName = "None"),
	Meat UMETA(DisplayName = "Meat"),
	Plant UMETA(DisplayName = "Plant")
};




