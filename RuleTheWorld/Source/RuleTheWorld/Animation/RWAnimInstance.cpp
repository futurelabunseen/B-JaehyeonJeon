// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/RWAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/RandomStream.h"

URWAnimInstance::URWAnimInstance()
{
	MovingThreshold = 3.0f;
	JumpingThreshold = 50.0f;
}

void URWAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if(Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
	
	RandomStream = FRandomStream();
}

void URWAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// MovementComponent에서 값을 받아옴
	if(Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D(); // z값을 제외한 XY의 크기
		bIsIdle = GroundSpeed < MovingThreshold;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshold); // Falling이면서 최소 Threshold 이상이면 점프 중
		
		// Focusing시의 좌우 이동방향 판별.
		FVector CrossProductResult = FVector::CrossProduct(Velocity, GetOwningActor()->GetActorForwardVector());
		CrossProductZ = CrossProductResult.Z;
		// 앞뒤 이동 판별
		DotProductValue = FVector::DotProduct(Velocity, GetOwningActor()->GetActorForwardVector());
	}
}
