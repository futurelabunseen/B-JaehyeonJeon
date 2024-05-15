// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RWAnimalAIController.h"
#include "RWAIWolfController.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWAIWolfController : public ARWAnimalAIController
{
	GENERATED_BODY()

public:
	ARWAIWolfController();

	virtual void OnPossess(APawn* InPawn) override;

};
