// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/RWAnimalBase.h"

#include "Component/RWAnimalStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Interface/RWTimeReachedInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Object/RWInteractableActor.h"

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
	
	// Set Meat Class
	static ConstructorHelpers::FClassFinder<ARWInteractableActor> MeatItemClassRef(TEXT("/Game/RuleTheWorld/Object/BP_RWItemMeat.BP_RWItemMeat_C"));
	if(MeatItemClassRef.Class)
	{
		MeatItemClass = MeatItemClassRef.Class;
	}
	
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
		
	IRWTimeReachedInterface* TimeReachedInterface = Cast<IRWTimeReachedInterface>(GetWorld()->GetAuthGameMode());
	if (TimeReachedInterface) // Interface로 받아온 Delegate에 등록
	{
		UE_LOG(LogTemp, Log, TEXT("Activate 바인딩 됨"));
		if (AnimalData != EAnimalData::Wolf)
		{
			TimeReachedInterface->GetOnTimeReachedMorning().AddDynamic(this, &ARWAnimalBase::ActivateAnimal);
		}
		else
		{
			TimeReachedInterface->GetOnTimeReachedNight().AddDynamic(this, &ARWAnimalBase::ActivateAnimal);
		}
	}
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

void ARWAnimalBase::SetDead() // 동물은 서버에서 스폰되니 서버에서 멀티 캐스트
{
	if(AnimalState == EAnimalState::Dead) return;

	AnimalState = EAnimalState::Dead;
	SpawnMeat();
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
	GetMesh()->SetVisibility(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARWAnimalBase::SpawnMeat()
{
	int32 CurrentRealTime;
	CurrentRealTime = FDateTime::Now().GetMillisecond();

	// RandomStream 인스턴스 생성 및 난수 생성
	FRandomStream RandomStream;
	RandomStream.Initialize(CurrentRealTime);

	FVector SpawnLocation = this->GetActorLocation();
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

	uint8 SpawnNum = RandomStream.RandRange(1, 4);
	for (int i = 0; i < SpawnNum; i++)
	{
		// 매 아이템 스폰 시 다른 지점에 떨어지도록
		CurrentRealTime = FDateTime::Now().GetMillisecond();
		RandomStream.Initialize(CurrentRealTime);

		FVector RandomOffset(RandomStream.FRandRange(-100.f, 100.f), RandomStream.FRandRange(-100.f, 100.f), 300.0f);
		FVector NewSpawnLocation = SpawnLocation + RandomOffset;

		// 바닥 찾기 위한 라인 트레이스 설정
		FVector Start = NewSpawnLocation + FVector(0.0f, 0.0f, 500.0f);  // 상단 500 유닛
		FVector End = NewSpawnLocation - FVector(0.0f, 0.0f, 500.0f);    // 하단 500 유닛

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		// 라인 트레이스 실행
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

		if (bHit)
		{
			// 바닥에 붙인 위치로 스폰 위치 조정
			NewSpawnLocation = HitResult.Location;
			NewSpawnLocation.Z += 30.0f; // 고기 아이템이 바닥에 붙도록 약간 위로 올림
		}
		
		UE_LOG(LogTemp, Log, TEXT("동물 처치 고기 생성 %f %f %f"), NewSpawnLocation.X, NewSpawnLocation.Y, NewSpawnLocation.Z);
		GetWorld()->SpawnActor<ARWInteractableActor>(MeatItemClass, NewSpawnLocation, SpawnRotation);
	}
}

void ARWAnimalBase::ActivateAnimal()
{
	UE_LOG(LogTemp, Log, TEXT("Activate Animal"))
	if(AnimalState != EAnimalState::Dead){ return; } // 죽었을 때만 동작
	
	AnimalState = EAnimalState::Live;

	// 최대 체력으로 설정
	StatComponent->SetHP(StatComponent->MaxHP); 

	// 위치 랜덤 위치로 변경
	// 현재 시간을 기반으로 시드 생성
	int32 CurrentRealTime = FDateTime::Now().GetMillisecond();
	
	// RandomStream 인스턴스 생성 및 난수 생성
	FRandomStream RandomStream;
	RandomStream.Initialize(CurrentRealTime);
			
	FVector NewLocation = FVector(RandomStream.FRandRange(-5000.f, 5000.f), RandomStream.FRandRange(-5000.f, 5000.f),  RandomStream.FRandRange(0.f, 100.f));

	SetActorLocation(NewLocation);

	// 클라이언트에서 에니메이션과 상태 변경
	NetMultiCastRPCActivateAnimal();
}

void ARWAnimalBase::NetMultiCastRPCActivateAnimal_Implementation()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// Visible하며 Collision이 가능하도록
	GetMesh()->SetVisibility(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Idle 모션 재생
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(IdleMontage, 1.0f);
}

