// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "RWTA_Trace.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWTA_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ARWTA_Trace();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;
	void SetShowDebug(bool InShowDebug) { bShowDebug = InShowDebug; };

protected:
	// Target에 대한 정보를 담는 Target Data가 모인 것 
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;

	bool bShowDebug = false;
};



