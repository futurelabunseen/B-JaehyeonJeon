// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/RWAnimationAttackInterface.h"
#include "RWAnimalBase.generated.h"

UCLASS()
class RULETHEWORLD_API ARWAnimalBase : public ACharacter, public IRWAnimationAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARWAnimalBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateWalkSpeed(float WalkSpeed);

// Stat : Attack & Damage
protected:
	TObjectPtr<class URWAnimalStatComponent> StatComponent;
	
	// RWAnimationAttackInterface 구현 
	virtual void AttackHitCheck() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackMontage;
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = RPC, Meta = (AllowPrivateAccess = "true"))
	void MulticastRPCAnimalAttack();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Dead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPCSetDead();

	void SetDead();

//Using Rifle Section 
public:
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastOnHitJump();
};
