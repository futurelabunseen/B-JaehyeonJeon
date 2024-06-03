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
	GetCharacterMovement()->JumpZVelocity = 700.f;
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
	if(!HasAuthority())
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

#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

#endif
}

float ARWAnimalBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	StatComponent->ApplyDamage(DamageAmount);
	
	if(HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("Server : %f"), StatComponent->CurrentHP);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Client : %f"), StatComponent->CurrentHP);
	}
	return DamageAmount;
}

void ARWAnimalBase::SetDead()
{
	NetMulticastRPCSetDead();
}

void ARWAnimalBase::NetMulticastOnHitJump_Implementation()
{
	this->Jump();
}

void ARWAnimalBase::MulticastRPCAnimalAttack_Implementation()
{
	// Animal Attack에 대한 Montage를 실행하도록 RPC를 전송
	PlayAnimMontage(AttackMontage);
}
void ARWAnimalBase::NetMulticastRPCSetDead_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
	
	GetCharacterMovement()->SetMovementMode(MOVE_None);
}





