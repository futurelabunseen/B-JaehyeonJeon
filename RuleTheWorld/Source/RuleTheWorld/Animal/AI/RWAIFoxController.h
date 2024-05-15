// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animal/AI/RWAnimalAIController.h"
#include "RWAIFoxController.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWAIFoxController : public ARWAnimalAIController
{
	GENERATED_BODY()

public:
	ARWAIFoxController();

	virtual void OnPossess(APawn* InPawn) override;
};
