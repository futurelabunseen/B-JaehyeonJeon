// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/AI/RWAnimalAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Interface/RWCharacterStateInterface.h"


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
	IRWCharacterStateInterface* PlayerInterface = Cast<IRWCharacterStateInterface>(Actor);
	if(PlayerInterface)
	{
		if(PlayerInterface->GetCharacterState() == ECharacterState::Dead)
		{
			return;
		}
		
		FName BoolKeyName = FName("HasLineOfSight");
		FName EnemyKeyName = FName("EnemyActor");
		BlackboardComponent->SetValueAsBool(BoolKeyName, true);
		BlackboardComponent->SetValueAsObject(EnemyKeyName, Actor);

		// 1초마다 플레이어의 상태를 체크하는 함수
		CheckCharacterState(Actor);
	}
	
}

// 죽은 캐릭터를 공격하지 않도록 설정
void ARWAnimalAIController::CheckCharacterState(AActor* Actor)
{
	IRWCharacterStateInterface* PlayerInterface = Cast<IRWCharacterStateInterface>(Actor);
	if(PlayerInterface->GetCharacterState() == ECharacterState::Dead)
	{
		FName BoolKeyName = FName("HasLineOfSight");
		FName EnemyKeyName = FName("EnemyActor");
		BlackboardComponent->SetValueAsBool(BoolKeyName, false);
		BlackboardComponent->ClearValue(EnemyKeyName);
	}
	else
	{
		FTimerHandle CharacterStateCheckTimerHandle;
		FTimerDelegate CheckCharacterStateDelegate;
		CheckCharacterStateDelegate.BindUFunction(this, FName("CheckCharacterState"), Actor);
        GetWorld()->GetTimerManager().SetTimer(
        	CharacterStateCheckTimerHandle,
        	CheckCharacterStateDelegate,
        	1.0f,
        	false
        );
	}
}

