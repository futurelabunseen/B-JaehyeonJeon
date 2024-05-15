// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/RWAnimNotify_AttackHitCheckGAS.h"
#include "AbilitySystemBlueprintLibrary.h"

URWAnimNotify_AttackHitCheckGAS::URWAnimNotify_AttackHitCheckGAS()
{
}

FString URWAnimNotify_AttackHitCheckGAS::GetNotifyName_Implementation() const
{
	return TEXT("AnimNotify : AttackHitCheck");
}

void URWAnimNotify_AttackHitCheckGAS::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if(MeshComp) // Character일 때,
	{
		AActor* OwnerActor = MeshComp->GetOwner(); // Load Actor
		if(OwnerActor) 
		{
			FGameplayEventData PayLoadData; // Empty 인자로 넣기 위해
			
			// ASC를 가진 Actor에 Event를 발동
			// TriggTriggerGamePlayTag(블루프린트에서 지정)를 가진 Ability를 Actor가 수행하도록 함
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGamePlayTag, PayLoadData);
		}
	}
}
