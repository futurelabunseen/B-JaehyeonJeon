// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "RWAnimNotify_AttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API URWAnimNotify_AttackHitCheck : public UAnimNotify
{
	GENERATED_BODY()


protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
