// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/RWAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/RandomStream.h"
#include "Net/UnrealNetwork.h"

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

	bIsRifleSet = false;
	bIsAiming = false;
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
		
		// 소유자 Pawn이 있는지 확인
		if (!OwningPawn)
		{
			OwningPawn = TryGetPawnOwner();
			if (OwningPawn)
			{
				OwningCharacter = Cast<ACharacter>(OwningPawn);
			}
		}
			
		// 소유자 Pawn이 유효한지 확인
		if (!OwningPawn) {
			return;
		}

		// 카메라 방향 가져오기
		//FRotator ControlRotation = PlayerController->GetControlRotation();
		FRotator ActorRotation = OwningPawn->GetActorRotation();

		// AimOffset 계산
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, ActorRotation);
		
		AimYaw = DeltaRot.Yaw;
		AimPitch = DeltaRot.Pitch;
	}
}
