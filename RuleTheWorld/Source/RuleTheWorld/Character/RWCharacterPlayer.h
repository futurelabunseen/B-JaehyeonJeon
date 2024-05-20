// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RWCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "RWCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWCharacterPlayer : public ARWCharacterBase
{
	GENERATED_BODY()

public:
	ARWCharacterPlayer();

protected:
	virtual void BeginPlay() override;
	// Multiplay
	virtual void OnRep_PlayerState() override;
	
	virtual void PossessedBy(AController* NewController) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
//Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> MiniMapSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USceneCaptureComponent2D> MiniMapSceneCapture;

// Attack Network
public:
	void Attack();

	virtual void AttackHitCheck() override;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAttack();
		
// Combo Attack Section
//protected:
	TObjectPtr<class UAnimInstance> AnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Animation)
	int32 CurrentCombo = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Animation)
	uint8 bHasNextComboCommand:1 = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Animation)
	uint8 bIsAttacking:1 = false;

	// ComboPunch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboAttackMontage1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboAttackMontage2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboAttackMontage3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboAttackMontage4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TArray<TObjectPtr<class UAnimMontage>> ComboAttackMontages;

	// ComboKick
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboKickMontage1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboKickMontage2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboKickMontage3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboKickMontage4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TArray<TObjectPtr<class UAnimMontage>> ComboKickMontages;
	
	void ComboActionProcessing();
	void PostComboAttack();
	FTimerHandle AttackTimerHandle;
	
};
