// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RWInventoryInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URWInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RULETHEWORLD_API IRWInventoryInterface
{
	GENERATED_BODY()
public:
	TObjectPtr<class ARWInteractableActor> CollisionedItem = nullptr;
	uint8 bIsItemInBound:1;
};
