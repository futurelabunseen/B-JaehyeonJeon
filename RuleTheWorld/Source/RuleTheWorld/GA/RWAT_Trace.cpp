// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/RWAT_Trace.h"

#include "TA/RWTA_Trace.h"
#include "AbilitySystemComponent.h"

URWAT_Trace::URWAT_Trace()
{
}

URWAT_Trace* URWAT_Trace::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<ARWTA_Trace> TargetActorClass)
{
	URWAT_Trace* NewTask = NewAbilityTask<URWAT_Trace>(OwningAbility);
	NewTask->TargetActorClass = TargetActorClass;
	return NewTask;
}

void URWAT_Trace::Activate()
{
	Super::Activate();

	SpawnInitializeTargetActor();
	FinalizeTargetActor();
	
	SetWaitingOnAvatar();
}

void URWAT_Trace::OnDestroy(bool bInOwnerFinished)
{
	if(SpawnedTargetActor)
	{
		SpawnedTargetActor->Destroy();
	}
	Super::OnDestroy(bInOwnerFinished);
}

void URWAT_Trace::SpawnInitializeTargetActor()
{
	SpawnedTargetActor = Cast<ARWTA_Trace>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	if(SpawnedTargetActor)
	{
		SpawnedTargetActor->SetShowDebug(true);
		SpawnedTargetActor->TargetDataReadyDelegate.AddUObject(this, &URWAT_Trace::OnTargetDataReadyCallback);
	}
}

void URWAT_Trace::FinalizeTargetActor()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if(ASC)
	{
		const FTransform SpawnTransform = ASC->GetAvatarActor()->GetTransform();
		SpawnedTargetActor->FinishSpawning(SpawnTransform);

		ASC->SpawnedTargetActors.Push(SpawnedTargetActor);
		SpawnedTargetActor->StartTargeting(Ability);
		// Confirm Targeting을 해주면 Target Actor에 있는 ConfirmTargetingAndContinue함수가 동작함
		SpawnedTargetActor->ConfirmTargeting();
	}
}

void URWAT_Trace::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	// 끝내는 부분
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast(DataHandle);
	}
	EndTask();
}
