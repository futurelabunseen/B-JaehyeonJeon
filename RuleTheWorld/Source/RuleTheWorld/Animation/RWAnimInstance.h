// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RWAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API URWAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	URWAnimInstance();

protected:
	// Initial
	virtual void NativeInitializeAnimation() override;
	// Tick
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class APlayerController> OwnerController;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> Movement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float GroundSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float MovingThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsIdle : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsJumping : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float JumpingThreshold;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	FRandomStream RandomStream;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float RandomKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bPlayerIsFocusing:1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float CrossProductZ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float DotProductValue;
};
