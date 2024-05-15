// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "RWAT_Trace.generated.h"


// DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTraceResultDelegate);
// 아래는 Parameter까지 받아오는 Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceResultDelegate, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);


/**
 * 
 */
UCLASS()
class RULETHEWORLD_API URWAT_Trace : public UAbilityTask
{
	GENERATED_BODY()

public:
	URWAT_Trace();
	
	// 자기 자신을 호출해서 Task를 만들어주는 Static 함수를 만든다.
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitForTrace", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static URWAT_Trace* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class ARWTA_Trace> TargetActorClass);
	
	//활성화 시
	virtual void Activate() override;
	
	// 삭제 시
	virtual void OnDestroy(bool bInOwnerFinished) override;

	void SpawnInitializeTargetActor();
	void FinalizeTargetActor();

protected:
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle);

public:
	// Task가 끝났을 때, 이 Task를 소유하고 있는 어빌리티에게 끝났음을 알리는 델리게이트가 필요
	UPROPERTY(BlueprintAssignable)
	FTraceResultDelegate OnComplete;

protected:
	// 클래스 값
	UPROPERTY()
	TSubclassOf<class ARWTA_Trace> TargetActorClass;
	// 실제 스폰시킨 액터를 보관
	UPROPERTY()
	TObjectPtr<class ARWTA_Trace> SpawnedTargetActor;
	
};
