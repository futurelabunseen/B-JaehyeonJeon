// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/Component/RWAnimalStatComponent.h"

// Sets default values for this component's properties
URWAnimalStatComponent::URWAnimalStatComponent()
{
	MaxHP = 200.f;
	CurrentHP = MaxHP;
}


// Called when the game starts
void URWAnimalStatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void URWAnimalStatComponent::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp<float>(NewHP, 0.0f, MaxHP);

	if(CurrentHP <= KINDA_SMALL_NUMBER) // 아주 작은 값?
	{
		// 사망 처리
		OnHPZero.Broadcast();
	}
}

float URWAnimalStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHP = CurrentHP;
	// 예측 가능한 값으로 처리하기 위해 변경
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);
	// CurrentHP를 Clamp해서 범위 내에 존재하도록 함
	
	SetHP(PrevHP - ActualDamage);

	return ActualDamage;
}


