// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RWRifleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RULETHEWORLD_API URWRifleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URWRifleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TScriptInterface<class IRWRifleActionInterface> RifleActionInterface;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	void SetReady();

	UPROPERTY(Replicated)
	uint8 bIsReadyToShoot:1 = false;

	UFUNCTION(Server, Reliable)
	void ReadyToShoot();
	UFUNCTION(Server, Reliable)
	void AbortReady();
	
	// 총 발사
	void Fire();

	void Reload();
	
	// 발사 거리
	float FireRange;
	// 총알 데미지
	float BulletDamage;

// Input Section
protected:
	void BindAction();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> ReadyToShootAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> ReloadAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> FireAction;
	
private:
	// 발사 트레이스 함수
	void PerformLineTrace();
};
