// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/RWAnimationAttackInterface.h"
#include "RWEnums.h"
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

	EAnimalState AnimalState;

	EAnimalData AnimalData = EAnimalData::None;

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

// Dead & Respawn
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dead, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Respawn, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> IdleMontage;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPCSetDead();

	void SetDead();

	UFUNCTION()
	void ActivateAnimal();
	void DeactivateAnimal();

	UFUNCTION(NetMulticast, Reliable)
	void NetMultiCastRPCActivateAnimal();

// Spawn Meat
public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<class ARWInteractableActor> MeatItemClass;
	
	void SpawnMeat();
};
