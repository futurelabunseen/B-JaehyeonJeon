// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/RWGA_AttackHitCheck.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GA/RWAT_Trace.h"
#include "GA/TA/RWTA_Trace.h"

URWGA_AttackHitCheck::URWGA_AttackHitCheck()
{
	// Instancing Option
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void URWGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	URWAT_Trace* AttackTraceTask = URWAT_Trace::CreateTask(this, ARWTA_Trace::StaticClass());

	AttackTraceTask->OnComplete.AddDynamic(this, &URWGA_AttackHitCheck::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();
}

void URWGA_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if(UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		// 데이터가 있는지
		FHitResult HitResult =  UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
		UE_LOG(LogTemp, Log, TEXT("RWGA_AHC %s Detected"), *(HitResult.GetActor()->GetName()));
	}
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled); 

}
