// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RWCharacterPlayer.h"


#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/RWPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/CanvasRenderTarget2D.h"


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
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	// Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Minimap SpringArm
	MiniMapSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MiniMapSpringArm"));
	MiniMapSpringArm->SetupAttachment(RootComponent);
	MiniMapSpringArm->TargetArmLength = 10000.0f;
	MiniMapSpringArm->bUsePawnControlRotation = false;
	MiniMapSpringArm->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	// Minimap Camera
	MiniMapSceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapSceneCaptrue"));
	MiniMapSceneCapture->SetupAttachment(MiniMapSpringArm, USpringArmComponent::SocketName);
	MiniMapSceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;

	
	
	// GAS
	// ASC is bring from PlayerState at PossessedBy()
	ASC = nullptr;
}

UAbilitySystemComponent* ARWCharacterPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

void ARWCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 미니맵에 사용 할 Canvas Render Target을 설정
	// 각 Client마다 생성된 이후에 지정해주어야 하기 때문에, BeginPlay에서 지정
	UCanvasRenderTarget2D* MinimapCanvasRenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(GetWorld(), UCanvasRenderTarget2D::StaticClass(), 1024, 1024);
	MiniMapSceneCapture->TextureTarget = MinimapCanvasRenderTarget;

	
}


void ARWCharacterPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
}

	void ARWCharacterPlayer::PossessedBy(AController* NewController)
{

	Super::PossessedBy(NewController);
	
	
	ARWPlayerState* RWPS = GetPlayerState<ARWPlayerState>();
	if(RWPS)
	{
		ASC = RWPS->GetAbilitySystemComponent();
		
		// Owner Actor와 Avatar Actor가 정해졌으니 초기화
		ASC->InitAbilityActorInfo(RWPS, this);
		

		for(const auto& StartAbility : StartAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartAbility);
			ASC->GiveAbility(StartSpec);
		}
		
		APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
		PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
	}
	
}



