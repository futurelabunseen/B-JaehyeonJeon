// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "RWPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARWPlayerState();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;;

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

};
