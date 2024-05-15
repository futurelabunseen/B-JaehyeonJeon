// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animal/AI/RWAnimalAIController.h"
#include "RWAIPigController.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWAIPigController : public ARWAnimalAIController
{
	GENERATED_BODY()

public:
	ARWAIPigController();

	virtual void OnPossess(APawn* InPawn) override;
	
};
