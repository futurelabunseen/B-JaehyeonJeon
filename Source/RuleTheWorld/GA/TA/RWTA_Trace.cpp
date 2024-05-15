// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/TA/RWTA_Trace.h"

#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

ARWTA_Trace::ARWTA_Trace()
{
}

void ARWTA_Trace::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	// 멤버변수 SourceActor
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void ARWTA_Trace::ConfirmTargetingAndContinue()
{
	if(SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		// 만들어진 TargetDataHandle을 보내는 작업
		TargetDataReadyDelegate.Broadcast(DataHandle);
		// Abiltiy Task에서 이 델리게이트를 구독하도록 하는 추가작업이 필요함
	}
}


FGameplayAbilityTargetDataHandle ARWTA_Trace::MakeTargetData() const
{
	ACharacter* Character = CastChecked<ACharacter>(SourceActor);

	// Hit의 정보가 담김
	FHitResult OutHitResult;
	const float AttackRange = 100.0f;
	const float AttackRadius = 50.f;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(UABTA_Trace), false, Character);
	const FVector Forward = Character->GetActorForwardVector();
	const FVector Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius(); 
	const FVector End = Start + Forward + AttackRange;

	// Radius 크기의 공을 만들어서 Start부터 End까지 밀고나가는 형태로 충돌을 판정
	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel1, FCollisionShape::MakeSphere(AttackRadius), Params);

	FGameplayAbilityTargetDataHandle DataHandle;
	if(HitDetected)
	{
		// AbilityTargetDataHandle에 정보를 넣어줘야 함
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult);
		DataHandle.Add(TargetData);
	}
	
#if ENABLE_DRAW_DEBUG
	if(bShowDebug)
	{
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		float CapsuleHalfHeight = AttackRange * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(Forward).ToQuat(), DrawColor, false, 5.0f);
	}
#endif	
	return DataHandle;

}
