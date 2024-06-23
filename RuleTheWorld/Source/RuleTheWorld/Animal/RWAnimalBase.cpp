// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/RWAnimalBase.h"

#include "Component/RWAnimalStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ARWAnimalBase::ARWAnimalBase()
{
	
	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(50.f,50.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -50.0f), FRotator(0.0f, -90.f,0.0f));
	//GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	// AI Controller Possess
	// AIControllerClass <- BP에서 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	StatComponent = CreateDefaultSubobject<URWAnimalStatComponent>(TEXT("Stat Component"));
	
	// Spawn시에 Collision이 있어도 생성되도록 설정 
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AnimalState = EAnimalState::Live;
}

// Called when the game starts or when spawned
void ARWAnimalBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARWAnimalBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StatComponent->OnHPZero.AddUObject(this, &ARWAnimalBase::SetDead);
}


void ARWAnimalBase::UpdateWalkSpeed(float WalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ARWAnimalBase::AttackHitCheck()
{
	if(!HasAuthority()) // 동물의 공격은 서버에서만 체크됨
	{
		UE_LOG(LogTemp, Log, TEXT("It's not has Authority"));
		return;
	}
	
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), true, this);

	const float AttackRange = 40.0f;
	const float AttackRadius = 50.0f;
	const float AttackDamage = 20.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(AttackRadius),Params);
	if(HitDetected)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

/*#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

#endif */
}

float ARWAnimalBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);


	if(AnimalState == EAnimalState::Dead)
	{
		return 0;
	}
	StatComponent->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void ARWAnimalBase::MulticastRPCAnimalAttack_Implementation()
{
	// Animal Attack에 대한 Montage를 실행하도록 RPC를 전송
	PlayAnimMontage(AttackMontage);
}

void ARWAnimalBase::SetDead() // 동물은 서버에서 스폰되니 서버에서 멀티
{
	if(AnimalState == EAnimalState::Dead) return;
	NetMulticastRPCSetDead();
}

void ARWAnimalBase::NetMulticastRPCSetDead_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
	
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ARWAnimalBase::DeactivateAnimal, 1.0f, false);
}

void ARWAnimalBase::DeactivateAnimal()
{
	AnimalState = EAnimalState::Dead;
	GetMesh()->SetVisibility(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARWAnimalBase::ActivateAnimal()
{
	UE_LOG(LogTemp, Log, TEXT("Activate Animal"))
	if(AnimalState != EAnimalState::Dead){ return; } // 죽었을 때만 동작
	
	AnimalState = EAnimalState::Live;
	
	GetMesh()->SetVisibility(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 위치 랜덤 위치로 변경
	// 현재 시간을 기반으로 시드 생성
	int32 CurrentRealTime = FDateTime::Now().GetMillisecond();
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(IdleMontage, 1.0f);

	// RandomStream 인스턴스 생성 및 난수 생성
	FRandomStream RandomStream;
	RandomStream.Initialize(CurrentRealTime);
			
	FVector SpawnLocation = FVector(RandomStream.FRandRange(-5000.f, 5000.f), RandomStream.FRandRange(-5000.f, 5000.f),  RandomStream.FRandRange(0.f, 100.f));

	SetActorLocation(SpawnLocation);

	// 클라이언트에서 에니메이션과 상태 변경
	NetMultiCastRPCActivateAnimal();
}

void ARWAnimalBase::NetMultiCastRPCActivateAnimal_Implementation()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(IdleMontage, 1.0f);
}

