// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RWAnimalStatComponent.generated.h"
DECLARE_MULTICAST_DELEGATE(FOnHPZeroDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RULETHEWORLD_API URWAnimalStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URWAnimalStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// HP Delegate
	FOnHPZeroDelegate OnHPZero;
	
	// VisibleInstanceOnly로 설정하면 각 Instance마다 다른 값을 설정할 수 있다.
	UPROPERTY(VisibleInstanceOnly, Category = "Stat")
	float MaxHP;
	// Transient 키워드로 디스크에 저장되지 않도록 구현할 수 있음
	UPROPERTY(BlueprintReadWrite, Transient, VisibleInstanceOnly, Category = "Stat")
	float CurrentHP;

	void SetHP(float NewHP);
	float ApplyDamage(float InDamage);
};
