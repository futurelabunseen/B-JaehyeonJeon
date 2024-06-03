// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/RWCharacterBase.h"
#include "Components/TimelineComponent.h"
#include "Interface/RWRifleActionInterface.h"
#include "RWCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWCharacterPlayer : public ARWCharacterBase, public IRWRifleActionInterface
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
	TObjectPtr<class URWAnimInstance> AnimInstance;
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


// Rifle Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rifle, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class URWRifleComponent> RifleComponent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Rifle, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class USkeletalMeshComponent> RifleMesh;

	virtual void AttachRifleToSocket(const FName& SocketName) override;
	virtual FVector GetFireStartLocation() override;


// Rifle 카메라 이동
public:
	// Timeline 컴포넌트
	UPROPERTY()
	TObjectPtr<class UTimelineComponent> CameraTimeline;

	// Timeline Curve
	UPROPERTY(EditAnywhere, Category = Camera)
	TObjectPtr<class UCurveFloat>  CameraCurve; // Assigned At BP

	void SetCameraTimeLine();

// Rifle action Interface

	virtual TObjectPtr<class URWAnimInstance> GetAnimInstance() override;
	// TimeLine으로 호출할 함수
	UFUNCTION()
	virtual void HandleTimelineProgress(float Value) override;

	virtual void StartAiming() override;
	virtual void StopAiming() override;
	
private:
	FVector DefaultCameraLocation;
	FVector AimingCameraLocation;
	FOnTimelineFloat ProgressFunction;
};
