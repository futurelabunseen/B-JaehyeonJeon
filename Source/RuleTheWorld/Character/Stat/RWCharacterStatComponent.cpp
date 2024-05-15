// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Stat/RWCharacterStatComponent.h"

// Sets default values for this component's properties
URWCharacterStatComponent::URWCharacterStatComponent()
{
	MaxHP = 200.f;
	CurrentHP = MaxHP;

	MaxHunger = 200.f;
	CurrentHunger = 0.f;
}


// Called when the game starts
void URWCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHP(MaxHP);
	SetHunger(0.f);
}

float URWCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHP = CurrentHP;
	// 예측 가능한 값으로 처리하기 위해 변경
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);
	// CurrentHP를 Clamp해서 범위 내에 존재하도록 함
	
	SetHP(PrevHP - ActualDamage);

	return ActualDamage;
}

void URWCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp<float>(NewHP, 0.0f, MaxHP);

	// HP가 바뀌었으니 처리하라고 MultiCast를 날려줌
	OnHPChanged.Broadcast(CurrentHP);

	if(CurrentHP <= KINDA_SMALL_NUMBER) // 아주 작은 값?
	{
		// 사망 처리
		OnHPZero.Broadcast();
	}
}

void URWCharacterStatComponent::SetHunger(float NewHunger)
{
	CurrentHunger = FMath::Clamp<float>(NewHunger, 0.0f, MaxHunger);

	// Hunger가 바뀌었으니 처리하라고 MultiCast를 날려줌
	OnHungerChanged.Broadcast(CurrentHunger);

	if(CurrentHunger >= MaxHunger) 
	{
		// 기아 상태가 되었다고 알림
		OnStarving.Broadcast();
	}
}


