// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/AI/RWAnimalAIController.h"

#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/RWCharacterBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


ARWAnimalAIController::ARWAnimalAIController()
{
	

	// AI Perception
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 3000; // 시야 반경 설정
	SightConfig->DetectionByAffiliation.bDetectEnemies = true; // 적 감지 설정
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// 시각 설정을 AIPerception 컴포넌트에 추가
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ARWAnimalAIController::OnTargetPerceptionUpdated);
	AIPerceptionComponent->ConfigureSense(*SightConfig);
}

void ARWAnimalAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	ARWCharacterBase* Player = Cast<ARWCharacterBase>(Actor);
	if(Player)
	{
		FName BoolKeyName = FName("HasLineOfSight");
		FName EnemyKeyName = FName("EnemyActor");
		BlackboardComponent->SetValueAsBool(BoolKeyName, true);
		BlackboardComponent->SetValueAsObject(EnemyKeyName, Player);
		
	}
}
