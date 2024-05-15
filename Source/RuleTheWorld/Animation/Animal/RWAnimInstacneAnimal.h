// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/RWAnimInstance.h"
#include "RWAnimInstacneAnimal.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API URWAnimInstacneAnimal : public URWAnimInstance
{
	GENERATED_BODY()

public:
	URWAnimInstacneAnimal();

protected:
	// Initial
	virtual void NativeInitializeAnimation() override;
	// Tick
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

};
