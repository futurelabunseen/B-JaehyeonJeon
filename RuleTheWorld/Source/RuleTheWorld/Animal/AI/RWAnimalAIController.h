// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RWAnimalAIController.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWAnimalAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARWAnimalAIController();
	
// BlackBoard & AI Perception
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BT)
	TObjectPtr<UBlackboardData> BlackboardAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BT)
	TObjectPtr<class UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BT)
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BT)
	TObjectPtr<class UBlackboardComponent> BlackboardComponent;	

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BT)
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
};
