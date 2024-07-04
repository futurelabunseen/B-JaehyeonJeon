// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/RWInteractableActor.h"
#include "RWItemMeat.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWItemMeat : public ARWInteractableActor
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
public:
	ARWItemMeat();

	
};
