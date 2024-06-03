// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RWRifleActionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URWRifleActionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RULETHEWORLD_API IRWRifleActionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void AttachRifleToSocket(const FName& SocketName) = 0;

	virtual FVector GetFireStartLocation() = 0;

	// 카메라 위치 전환 함수
	virtual void HandleTimelineProgress(float Value) = 0;

	virtual void StartAiming() = 0;
	virtual void StopAiming() = 0;

	virtual TObjectPtr<class URWAnimInstance> GetAnimInstance() = 0;
};
