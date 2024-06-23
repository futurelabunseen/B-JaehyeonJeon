// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Stat/RWCharacterStatComponent.h"

#include "RWEnums.h"
#include "Interface/RWCharacterStateInterface.h"
#include "Net/UnrealNetwork.h"

constexpr float HUNGER_INCREASE_RATE = 1.f;

constexpr float STARVING_DAMAGE = 1.f;

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

	OwnerActor = GetOwner();
	if(OwnerActor->HasAuthority())
	{
		IncreaseHungerOverTime();	
	}
}

void URWCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URWCharacterStatComponent, CurrentHP);
	DOREPLIFETIME(URWCharacterStatComponent, CurrentHunger);
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

	// 리슨서버의 경우 Replication되지 않아 서버일 경우 명시적으로 실행
	if(GetOwner()->HasAuthority())
	{
		OnRep_CurrentHP();
	}
}

void URWCharacterStatComponent::SetHunger(float NewHunger)
{
	CurrentHunger = FMath::Clamp<float>(NewHunger, 0.0f, MaxHunger);
	
	// 리슨서버의 경우 Replication되지 않아 서버일 경우 명시적으로 실행
	if(GetOwner()->HasAuthority())
	{
		OnRep_CurrentHunger();
	}

	// 배고픔 수치가 최대로 도달했을 때
	if(CurrentHunger >= MaxHunger)
	{
		ApplyStarvingDamage();
	}
}

void URWCharacterStatComponent::OnRep_CurrentHP()
{
	// HP가 바뀌었으니 처리하라고 Delegate를 날려줌
	OnHPChanged.Broadcast(CurrentHP);

	if(CurrentHP <= KINDA_SMALL_NUMBER) // 아주 작은 값?
	{
		// 사망 처리
		OnHPZero.Broadcast();
	}


	UE_LOG(LogTemp, Log, TEXT("HP Replicated"));		
}

void URWCharacterStatComponent::OnRep_CurrentHunger()
{
	// Hunger가 바뀌었으니 처리하라고 Delegate를 날려줌
	OnHungerChanged.Broadcast(CurrentHunger);

	if(CurrentHunger >= MaxHunger) 
	{
		// 기아 상태가 되었다고 알림
		OnStarving.Broadcast();
	}
}

void URWCharacterStatComponent::IncreaseHungerOverTime()
{
	SetHunger(CurrentHunger + HUNGER_INCREASE_RATE);

	FTimerHandle IncreaseHungerTimerHandle;

	IRWCharacterStateInterface* CharacterStateInterface = Cast<IRWCharacterStateInterface>(OwnerActor);
	if(ECharacterState::Dead != CharacterStateInterface->GetCharacterState())
	{
		GetWorld()->GetTimerManager().SetTimer(
			IncreaseHungerTimerHandle,
			this,
			&URWCharacterStatComponent::IncreaseHungerOverTime,
			1.f,
			false
		);
	}
}

void URWCharacterStatComponent::ApplyStarvingDamage()
{
	if(CurrentHunger < MaxHunger || CurrentHP <= 0)
	{
		return;	
	}

	// 서버에서 데미지 처리 수행
	if(GetOwner()->HasAuthority())
	{
		ApplyDamage(STARVING_DAMAGE);
	}
	
	FTimerHandle StarvingDamageTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		StarvingDamageTimerHandle,
		this,
		&URWCharacterStatComponent::ApplyStarvingDamage,
		1.f,
		false
	);
}


