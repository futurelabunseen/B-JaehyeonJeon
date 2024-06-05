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

	TObjectPtr<class ACharacter> OwnerPlayer;
public:
	void SetReady();

	// 총 들기 
	UPROPERTY(Replicated)
	uint8 bIsReadyToShoot:1 = false;
	UFUNCTION(Server, Reliable)
	void ServerRPCReadyToShoot();
	UFUNCTION(Server, Reliable)
	void ServerRPCAbortReady();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPCAnimReadyToShoot(uint8 _bIsReadyToShoot);

	void SetAnimReadyToShoot(uint8 _bIsReadyForShoot);
	void SetAnimAiming(uint8 _bIsAiming);

	// 조준하기
	UPROPERTY(Replicated)
	uint8 bIsAiming:1 = false;

	void StartAiming();
	void StopAiming();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCStartAiming();
	UFUNCTION(Server, Reliable)
	void ServerRPCCompleteAiming();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPCSetAiming(uint8 _bIsAiming);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Aiming)
	TSubclassOf<class UUserWidget> CrossHairWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Aiming)
	TObjectPtr<class UUserWidget> CrossHairWidgetInstance;
	
	// 총 발사
	void Fire();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> RifleFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	TObjectPtr<class USoundWave> FireSound;


	// 총탄 및 장전
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Sound)
	uint8 BulletNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Sound)
	uint8 LoadedBullet;

	void Reload();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCReload();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPCReload();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ReloadMontage;

	
	
	// 발사 거리
	float FireRange;
	// 총알 데미지
	float BulletDamage;

// Input Section
protected:
	void BindAction();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> AimingAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> ReadyToShootAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> ReloadAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> FireAction;
	
private:
	// 발사 트레이스 함수
	UFUNCTION(Server, Reliable)
	void ServerRPCPerformLineTrace(FVector CameraTraceStart, FVector CameraTraceEnd);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPCShootFire();
};


