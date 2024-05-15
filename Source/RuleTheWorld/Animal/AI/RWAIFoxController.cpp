// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/AI/RWAIFoxController.h"

ARWAIFoxController::ARWAIFoxController()
{
	// Black Board
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BlackBoardRef(TEXT("/Script/AIModule.BlackboardData'/Game/RuleTheWorld/Animal/AI/BB/BB_Fox.BB_Fox'"));
	if(BlackBoardRef.Object)
	{
		BlackboardAsset = BlackBoardRef.Object;
	}
	
	// Behavior Tree
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeRef(TEXT("/Script/AIModule.BehaviorTree'/Game/RuleTheWorld/Animal/AI/BT/BT_Fox.BT_Fox'"));
	if(BehaviorTreeRef.Object)
	{
		BehaviorTreeAsset = BehaviorTreeRef.Object;
	}
}

void ARWAIFoxController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UBlackboardComponent* TempBlackBoard = Blackboard.Get(); 
	if(UseBlackboard(BlackboardAsset, TempBlackBoard))
	{
		if(!RunBehaviorTree(BehaviorTreeAsset))
		{
			UE_LOG(LogTemp, Error, TEXT("Fox AI Controller : BehaviorTree is not Running"));
		}
		
		BlackboardComponent = GetBlackboardComponent();
	}
}
