// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RWPlayerState.h"
#include "AbilitySystemComponent.h"



ARWPlayerState::ARWPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    ASC->SetIsReplicated(true);
}

UAbilitySystemComponent* ARWPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
