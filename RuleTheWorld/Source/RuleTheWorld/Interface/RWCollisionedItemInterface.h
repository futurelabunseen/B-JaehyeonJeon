// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RWCollisionedItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URWCollisionedItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RULETHEWORLD_API IRWCollisionedItemInterface
{
	GENERATED_BODY()
public:
	TObjectPtr<class ARWInteractableActor> CollisionedItem = nullptr;
	uint8 bIsItemInBound:1;
};
