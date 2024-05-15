// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RWGA_AttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API URWGA_AttackHitCheck : public UGameplayAbility
{
	GENERATED_BODY()

public:
	URWGA_AttackHitCheck();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:

	UFUNCTION()
	void OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

};
