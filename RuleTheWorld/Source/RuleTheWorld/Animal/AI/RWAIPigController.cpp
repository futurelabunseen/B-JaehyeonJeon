// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/AI/RWAIPigController.h"

ARWAIPigController::ARWAIPigController()
{
	// Black Board
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BlackBoardRef(TEXT("/Script/AIModule.BlackboardData'/Game/RuleTheWorld/Animal/AI/BB/BB_Pig.BB_Pig'"));
	if(BlackBoardRef.Object)
	{
		BlackboardAsset = BlackBoardRef.Object;
	}
	
	// Behavior Tree
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeRef(TEXT("/Script/AIModule.BehaviorTree'/Game/RuleTheWorld/Animal/AI/BT/BT_Pig.BT_Pig'"));
	if(BehaviorTreeRef.Object)
	{
		BehaviorTreeAsset = BehaviorTreeRef.Object;
	}
}

void ARWAIPigController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UBlackboardComponent* TempBlackBoard = Blackboard.Get(); 
	if(UseBlackboard(BlackboardAsset, TempBlackBoard))
	{
		if(!RunBehaviorTree(BehaviorTreeAsset))
		{
			UE_LOG(LogTemp, Error, TEXT("Pig AI Controller : BehaviorTree is not Running"));
		}
		
		BlackboardComponent = GetBlackboardComponent();
	}
}
