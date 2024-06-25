// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RWCharacterPlayer.h"


#include "Animation/RWAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Net/UnrealNetwork.h"
#include "Riple/RWRifleComponent.h"

constexpr int32 MAX_COMBO = 3;

ARWCharacterPlayer::ARWCharacterPlayer()
{
	// Anim Instance Assign
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/RuleTheWorld/Character/Animation/ABP_RWChracter.ABP_RWChracter_C"));
	if(AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}
	
	// Spring Arm
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 550.f;
	CameraBoom->bUsePawnControlRotation = true;

	// Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Minimap SpringArm
	MiniMapSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MiniMapSpringArm"));
	MiniMapSpringArm->SetupAttachment(RootComponent);
	MiniMapSpringArm->TargetArmLength = 1000.0f;
	MiniMapSpringArm->bUsePawnControlRotation = false;
	MiniMapSpringArm->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	// Minimap Camera
	MiniMapSceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapSceneCaptrue"));
	MiniMapSceneCapture->SetupAttachment(MiniMapSpringArm, USpringArmComponent::SocketName);
	MiniMapSceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	
	// Rifle
	RifleComponent = CreateDefaultSubobject<URWRifleComponent>(TEXT("Rifle"));
	RifleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RifleSkeletalMesh"));

	// Rile Camera Move
	CameraTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CameraTimeline"));
	DefaultCameraLocation = FVector(0.0f, 0.0f, 0.0f);
	AimingCameraLocation = FVector(0.0f, 60.0f, 60.0f);
}



void ARWCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();


	if(IsLocallyControlled())
	{
		// 미니맵에 사용 할 Canvas Render Target을 설정
		// 각 Client마다 생성된 이후에 지정해주어야 하기 때문에, BeginPlay에서 지정
		UCanvasRenderTarget2D* MinimapCanvasRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(GetWorld(), UCanvasRenderTarget2D::StaticClass(), 1024, 1024);
		MiniMapSceneCapture->TextureTarget = MinimapCanvasRenderTarget;
	
	}
	else
	{
		MiniMapSceneCapture->SetActive(false);
	}
	
	// Get AnimInstance
	AnimInstance = Cast<URWAnimInstance>(GetMesh()->GetAnimInstance());

	// ComboAttack
	ComboAttackMontages = {ComboAttackMontage1, ComboAttackMontage2, ComboAttackMontage3, ComboAttackMontage4};
	ComboKickMontages = {ComboKickMontage1, ComboKickMontage2, ComboKickMontage3, ComboKickMontage4};

	// Camera Move
	SetCameraTimeLine();
}




void ARWCharacterPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
}

void ARWCharacterPlayer::PossessedBy(AController* NewController)
{

	Super::PossessedBy(NewController);
	
}

void ARWCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARWCharacterPlayer, CurrentCombo);
	DOREPLIFETIME(ARWCharacterPlayer, bHasNextComboCommand);
	
	DOREPLIFETIME(ARWCharacterPlayer, bIsAttacking);
}

void ARWCharacterPlayer::Attack()
{
	if(JumpCurrentCount > 0) // Jump 중엔 수행하지 않음
	{
		return;
	}
	
	// 총을 꺼내고 있는 경우에는 총격
	if(RifleComponent->bIsReadyToShoot)
	{
		RifleComponent->Fire();
	}
	else // 아닌 경우 일반 공격
	{
		ServerRPCAttack();
	}
}

void ARWCharacterPlayer::AttackHitCheck()
{
	Super::AttackHitCheck();
}

void ARWCharacterPlayer::ServerRPCAttack_Implementation()
{
	MulticastRPCAttack();
}

bool ARWCharacterPlayer::ServerRPCAttack_Validate()
{
	return true;
}

void ARWCharacterPlayer::MulticastRPCAttack_Implementation()
{
	// 공격 중이 아닐 때, Animation 재생
	if(!bIsAttacking)
	{
		// Animation Setting
		const float AttackSpeedRate = 1.0f;
	
		if(bIsAnimalInBound)
		{ // 근처에 동물이 있다면 발차기 공격을 수행
			AnimInstance->Montage_Play(ComboKickMontages[CurrentCombo], AttackSpeedRate);
		}
		else
		{ // 그게 아니면 일반 공격(펀치) 수행
			AnimInstance->Montage_Play(ComboAttackMontages[CurrentCombo], AttackSpeedRate);
		}
	}
	// 서버에서 콤보액션 관련 변수를 처리
	if(HasAuthority())
	{
		ComboActionProcessing();
	}
}

// Server Only
// 서버에서 콤보액션과 관련한 변수들을 관리하도록 하는 함수
void ARWCharacterPlayer::ComboActionProcessing()
{
	if(bIsAttacking)
	{
		return;
	}
	
	bIsAttacking = true;

	// Movement Setting
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	
	// Combo Status
	float RateTime; 
	if(CurrentCombo == MAX_COMBO)
	{
		CurrentCombo = 1;
		RateTime = 1.8f;
	}
	else
	{
		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, MAX_COMBO);
		RateTime = 1.0f;
	}
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ARWCharacterPlayer::PostComboAttack, RateTime, false);
}

// Server Only
// 서버에서 콤보액션과 관련한 변수들을 관리하도록 하는 함수 (공격 후처리)
void ARWCharacterPlayer::PostComboAttack()
{
	AttackTimerHandle.Invalidate();
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	
	// 공격 상태 해제
	bIsAttacking = false;
	
	if(!bHasNextComboCommand) // Non Attack
	{
		CurrentCombo = 0;
	}
	else // Next Attack
	{
		bHasNextComboCommand = false;
	}
}

void ARWCharacterPlayer::AttachRifleToSocket(const FName& SocketName)
{
	USkeletalMeshComponent* OwnerMesh = GetMesh();
	if (OwnerMesh)
	{
		// Owner의 스켈레탈 메시에 소켓이 존재하는지 확인
		if (OwnerMesh->DoesSocketExist(SocketName))
		{
			// Gun을 소켓에 부착
			RifleMesh->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
		}
	}
}

// 총알 발사된 지점 
FVector ARWCharacterPlayer::GetFireStartLocation()
{
	FName SocketName(TEXT("FireStartPoint"));
	return GetMesh()->GetSocketLocation(SocketName);
}

void ARWCharacterPlayer::HandleTimelineProgress(float Value)
{
	FVector NewLocation = FMath::Lerp(DefaultCameraLocation, AimingCameraLocation, Value);
	FollowCamera->SetRelativeLocation(NewLocation);

	CameraBoom->TargetArmLength = FMath::Lerp(550.f, 150.f, Value);
}

void ARWCharacterPlayer::StartAiming()
{
	if (CameraTimeline)
	{
		CameraTimeline->PlayFromStart();
	}
}

void ARWCharacterPlayer::StopAiming()
{
	if (CameraTimeline)
	{
		CameraTimeline->Reverse();
	}
}


void ARWCharacterPlayer::SetCameraTimeLine()
{
	if (CameraCurve)
	{
		// Progress function 바인딩
		ProgressFunction.BindUFunction(this, FName("HandleTimelineProgress"));

		// Timeline 설정
		CameraTimeline->AddInterpFloat(CameraCurve, ProgressFunction);
		CameraTimeline->SetLooping(false);
	}
}

// Rifle에서 AnimInstance를 받기 위해
TObjectPtr<URWAnimInstance> ARWCharacterPlayer::GetAnimInstance()
{
	return AnimInstance;
}
