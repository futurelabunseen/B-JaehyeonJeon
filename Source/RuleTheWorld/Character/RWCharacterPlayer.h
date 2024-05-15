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
class RULETHEWORLD_API ARWCharacterPlayer : public ARWCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARWCharacterPlayer();
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	virtual void BeginPlay() override;
	// Multiplay
	virtual void OnRep_PlayerState() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
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


	
protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;
	
	// 처음부터 부여할 어빌리티dml 목록 <- 내용물은 블루프린트에서 설정
	UPROPERTY(EditAnywhere, Category=GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;
};
