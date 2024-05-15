// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RWCharacterStatComponent.generated.h"

// Multicast로 여러 상대에게 보냄, 
DECLARE_MULTICAST_DELEGATE(FOnHPZeroDelegate);
// 하나의 인자를 넣어서 델리게이트를 보냄
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChangedDelegate, float /*CurrentHP*/)

// 굶주림
DECLARE_MULTICAST_DELEGATE(FOnStarvingDelegate);
// Change Hunger Stat
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHungerChangedDelegate, float /*CurrentHunger*/)




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RULETHEWORLD_API URWCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URWCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// HP Delegate
	FOnHPZeroDelegate OnHPZero;
	FOnHPChangedDelegate OnHPChanged;
	// Hunger Delegate
	FOnStarvingDelegate OnStarving;
	FOnHungerChangedDelegate OnHungerChanged;

	
	FORCEINLINE float GetMaxHP() { return MaxHP; }
	FORCEINLINE float GetCurrentHP() { return CurrentHP; }
	// 데미지 처리
	float ApplyDamage(float InDamage);
	
	FORCEINLINE float GetMaxHunger() { return MaxHunger; }
	FORCEINLINE float GetCurrentHunger() { return CurrentHunger; }

protected:
	// HP를 설정, 델리게이트 호출
	void SetHP(float NewHP);
	// Hunger를 설정, 델리게이트 호출
	void SetHunger(float NewHunger);
	
	// VisibleInstanceOnly로 설정하면 각 Instance마다 다른 값을 설정할 수 있다.
	UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	float MaxHP;
	// Transient 키워드로 디스크에 저장되지 않도록 구현할 수 있음
	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat")
	float CurrentHP;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	float MaxHunger;
	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat")
	float CurrentHunger;

	
};
