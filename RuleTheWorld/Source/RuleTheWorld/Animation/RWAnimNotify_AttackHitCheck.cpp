// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/RWAnimNotify_AttackHitCheck.h"
#include "Interface/RWAnimationAttackInterface.h"

void URWAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if(MeshComp)
	{
		// Owner가 Interface에 구현이 되지 않았다면 NULL이 아니게 됨.
		// 그래서 구현되었는지 Check를 Null인지로 판별 함. 
		IRWAnimationAttackInterface* AttackPawn = Cast<IRWAnimationAttackInterface>(MeshComp->GetOwner());
		if(AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}
}
