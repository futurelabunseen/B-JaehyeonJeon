// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RWCharacterBase.h"

#include "Animal/RWAnimalBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"
#include "Object/RWInteractableActor.h"
#include "Stat/RWCharacterStatComponent.h"
#include "NiagaraComponent.h"
#include "UI/RWMainWidget.h"

constexpr float OUTDOOR_FIRE_DAMAGE = 3.0f;

// Sets default values
ARWCharacterBase::ARWCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	CharacterState = ECharacterState::Live;
	
	// Pawn의 Rotation?
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;


	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(30.f,96.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 420.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	
	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.f,0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	// Assign Mesh Reference 
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Asset/survivor_man/Mesh/Survivor_man_3/SK_Survivor_man_3_NoCloth.SK_Survivor_man_3_NoCloth'"));
	if(CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}


	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/RuleTheWorld/Character/Animation/Animations/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	// Item
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->InitBoxExtent(FVector(50.f, 50.f,50.f));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	// Set Overlap Delegate
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ARWCharacterBase::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ARWCharacterBase::OnOverlapEnd);
	CollisionBox->SetupAttachment(RootComponent);
	CollisionedItem = nullptr;
	bIsItemInBound = false;

	// CollisionBox Detection
	CollisionedPawn = nullptr;
	bIsAnimalInBound = false;

	// Stat Component
	StatComponent = CreateDefaultSubobject<URWCharacterStatComponent>(TEXT("Stat"));
	StatComponent->SetIsReplicated(true);

}

void ARWCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Components가 초기화 된 이후 Niagara 컴포넌트를 받아옴
	FName FireNiagaraTag(TEXT("FireNiagara"));
	UActorComponent* FireNiagaraActorComponent = FindComponentByTag(UNiagaraComponent::StaticClass(), FireNiagaraTag);
	if(FireNiagaraActorComponent)
	{
		NiagaraEffectFire = Cast<UNiagaraComponent>(FireNiagaraActorComponent);
	}
	
	if(NiagaraEffectFire)
	{
		NiagaraEffectFire->SetVisibility(false);
	}
	
	// HP가 0이 될 경우 사망 -> Delegate로 Stat Component와 연결 
	StatComponent->OnHPZero.AddUObject(this, &ARWCharacterBase::SetDead);
	// Hungry가 Max가 되면 Staring으로 변경 -> -> Delegate로 Stat Component와 연결
	StatComponent->OnStarving.AddUObject(this, &ARWCharacterBase::SetStarving);


	UE_LOG(LogTemp, Log, TEXT("로그 : 플레이어 생성"));
}

void ARWCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARWCharacterBase, bIsInShelter);
}

ECharacterState ARWCharacterBase::GetCharacterState()
{
	return CharacterState;
}

void ARWCharacterBase::SetCharacterState(ECharacterState NewState)
{
	// CharacterState 변경
	if (CharacterState != NewState)
	{
		CharacterState = NewState;
	}
}

void ARWCharacterBase::SetUpCharacterWidget(URWMainWidget* MainWidget)
{
	//IRWCharacterWidgetInterface::SetUpCharacterWidget(MainWidget);
	
	// StatComponent에서 값을 얻어와 설정해줌
	MainWidget->SetMaxHP(StatComponent->GetMaxHP());
	MainWidget->SetMaxHunger(StatComponent->GetMaxHunger());

	// StatBar Initialize
	MainWidget->UpdateHPBar(StatComponent->GetCurrentHP());
	MainWidget->UpdateHungerBar(StatComponent->GetCurrentHunger());

	// Delegate 등록
	StatComponent->OnHPChanged.AddUObject(MainWidget, &URWMainWidget::UpdateHPBar);
	StatComponent->OnHungerChanged.AddUObject(MainWidget, &URWMainWidget::UpdateHungerBar);
}

void ARWCharacterBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == this)
	{
		return;
	}
	
	// 잡고있는 item이 없고, RWInteractableActor인 경우
	ARWInteractableActor* OtherInteractableActor = Cast<ARWInteractableActor>(OtherActor);
	if(OtherInteractableActor)
	{
		// Inventory 구현부
		CollisionedItem = OtherInteractableActor;
		bIsItemInBound = true;
	}
	else
	{ // Item이 아니라면 Pawn인지 확인
		
		APawn* OtherPawn = Cast<APawn>(OtherActor);
		if(OtherPawn)
		{
			CollisionedPawn = OtherPawn;
			if(OtherActor->IsA<ARWAnimalBase>())
			{
				bIsAnimalInBound = true;
			}
		}
	}
}

void ARWCharacterBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Bounding Box 내에 Collision 되었던 것이 Item인 경우
	ARWInteractableActor* OtherInteractableActor = Cast<ARWInteractableActor>(OtherActor);
	if(OtherInteractableActor == CollisionedItem && OtherInteractableActor != nullptr)
	{
		CollisionedItem = nullptr;
		bIsItemInBound = false;
	}
	else
	{
		APawn* OtherPawn = Cast<APawn>(OtherActor);
		if(OtherPawn == CollisionedPawn && OtherPawn != nullptr)
		{
			if(CollisionedPawn->IsA<ARWAnimalBase>())
			{
				bIsAnimalInBound = false;
			}	
			CollisionedPawn = nullptr;
		}
	}
}

TObjectPtr<ARWInteractableActor> ARWCharacterBase::GetCollisionedItem()
{
	return CollisionedItem;
}

uint8 ARWCharacterBase::GetIsItemInBound()
{
	return bIsItemInBound;
}

void ARWCharacterBase::AttackHitCheck()
{
	
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), true, this);

	const float AttackRange = 40.0f;
	const float AttackRadius = 50.0f;
	const float AttackDamage = 30.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(AttackRadius),Params);
	if(HitDetected)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

/*
#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

#endif
*/
	
}

float ARWCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	// EventInstigator -> 나에게 피해를 입힌 Controller, DamageCauser -> 사용한 무기 또는 빙의한 폰 (데미지를 준) 
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if(this->CharacterState == ECharacterState::Dead) // 캐릭터가 죽어있는 경우에는 공격받지 
	{
		return 0.f;
	}
	
	// Damage 받은 것을 StatComponent에서 적용시킴
	StatComponent->ApplyDamage(DamageAmount);

	// 죽지 않은 경우 Heart 모션
	if (StatComponent->GetCurrentHP() > 0)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);
		PlayAnimMontage(HeartMontage);
		MulticastRPCTakeDamageReaction();

		// Lambda function to restore movement after 1 second
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}, 1.0f, false);
	}
	
	return DamageAmount;
}

void ARWCharacterBase::MulticastRPCTakeDamageReaction_Implementation()
{
	PlayAnimMontage(HeartMontage);
}

void ARWCharacterBase::SetDead()
{
	// 캐릭터가 가지고 있는 EnhancedInputComponent 가져오기
	ServerRPCSetDead();
}

void ARWCharacterBase::ServerRPCSetDead_Implementation()
{
	MulticastRPCSetDead();
}

void ARWCharacterBase::MulticastRPCSetDead_Implementation()
{
	PlayDeadAnimation();
	// 사망 시 이동 제한
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	// 사망 시 State 교체 및 불 이펙트 종료
	SetCharacterState(ECharacterState::Dead);
	NiagaraEffectFire->Deactivate();
	if(NiagaraEffectFire->IsVisible())
	{
		NiagaraEffectFire->SetVisibility(false);	
	}

	// 더 이상 충돌되지 않도록 
	SetActorEnableCollision(false);
}

void ARWCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}

void ARWCharacterBase::SetStarving()
{
	// 기아 상태 시 변화 구현 필요
}

void ARWCharacterBase::SetupShelterEntry()
{
	bIsInShelter = true;
	NiagaraEffectFire->SetVisibility(false);
}

void ARWCharacterBase::SetupShelterExit()
{
	bIsInShelter = false;
	NiagaraEffectFire->SetVisibility(true);
	if(HasAuthority())
	{
		ApplyFireDamageOverTime();
	}
}

void ARWCharacterBase::ApplyFireDamageOverTime()
{
	// 죽었거나 Shelter안으로 들어간 경우 동작하지 않음 <- Timer로 다시 호출되는 경우에 핵심
	if(bIsInShelter || GetCharacterState() == ECharacterState::Dead)
	{
		return;	
	}
	
	// 데미지 적용
	StatComponent->ApplyDamage(OUTDOOR_FIRE_DAMAGE);

	// 1초 마다 다시 수행되도록
	FTimerHandle FireDamageOverTimeTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		FireDamageOverTimeTimerHandle,
		this,
		&ARWCharacterBase::ApplyFireDamageOverTime,
		1.f,
		false
	);
}


