// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RWPlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"

#include "RWEnums.h"
#include "Character/RWCharacterPlayer.h"
#include "Character/Inventory/RWInventoryComponent.h"
#include "Object/RWInteractableActor.h"


ARWPlayerController::ARWPlayerController()
{
	//Input
	bIsEnableLook = false;
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/RuleTheWorld/Input/IMC_Default.IMC_Default'"));
	if(nullptr != InputMappingContextRef.Object)
	{
		DefaultMappingContext = InputMappingContextRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_Move.IA_Move'"));
	if(nullptr != InputActionMoveRef.Object)
	{
		MoveAction = InputActionMoveRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_Jump.IA_Jump'"));
	if(nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_Look.IA_Look'"));
	if(nullptr != InputActionLookRef.Object)
	{
		LookAction = InputActionLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionEnableLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_EnableLook.IA_EnableLook'"));
	if(nullptr != InputActionEnableLookRef.Object)
	{
		EnableLookAction = InputActionEnableLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionRunRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_Run.IA_Run'"));
	if(nullptr != InputActionRunRef.Object)
	{
		RunAction = InputActionRunRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_Attack.IA_Attack'"));
	if(nullptr != InputActionAttackRef.Object)
	{
		AttackAction = InputActionAttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSneakingRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_Sneaking.IA_Sneaking'"));
	if(nullptr != InputActionSneakingRef.Object)
	{
		SneakingAction = InputActionSneakingRef.Object;
	}
	
	

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFocusingRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_Focusing.IA_Focusing'"));
	if(nullptr != InputActionFocusingRef.Object)
	{
		FocusingAction = InputActionFocusingRef.Object;
	}
	
	// Inventory
	InventoryComponent = CreateDefaultSubobject<URWInventoryComponent>(TEXT("Inventory"));

	bReplicates = true;
	SetReplicates(true);
}


void ARWPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작과 동시에 마우스 인풋을 게임 내부로 이동시키기
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	// Player
	PlayerPawn = Cast<ARWCharacterPlayer>(GetPawn());


	// Local Controller인 경우에만
	if(IsLocalController())
	{
		// UI 생성
		HUDInstancing();
		HUDAddToViewport();
	}
	
}


void ARWPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	// 함수와 바인딩
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ARWPlayerController::ControllerJump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARWPlayerController::ControllerStopJumping);
	EnhancedInputComponent->BindAction(EnableLookAction, ETriggerEvent::Triggered, this, &ARWPlayerController::EnableLook);
	EnhancedInputComponent->BindAction(EnableLookAction, ETriggerEvent::Completed, this, &ARWPlayerController::DisableLook);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARWPlayerController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARWPlayerController::Look);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ARWPlayerController::Run);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ARWPlayerController::StopRunning);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ARWPlayerController::Attack);
	EnhancedInputComponent->BindAction(SneakingAction, ETriggerEvent::Triggered, this, &ARWPlayerController::Sneaking);
	EnhancedInputComponent->BindAction(SneakingAction, ETriggerEvent::Completed, this, &ARWPlayerController::StopSneaking);
	EnhancedInputComponent->BindAction(FocusingAction, ETriggerEvent::Triggered, this, &ARWPlayerController::Focusing);
	EnhancedInputComponent->BindAction(FocusingAction, ETriggerEvent::Completed, this, &ARWPlayerController::StopFocusing);
}

void ARWPlayerController::PostNetInit()
{
	Super::PostNetInit();
}

void ARWPlayerController::HUDInstancing()
{
	if (HUDWidgetClass != nullptr)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
	}
}

void ARWPlayerController::HUDAddToViewport()
{
	if (HUDWidgetInstance != nullptr)
	{
		HUDWidgetInstance->AddToViewport();
	}
}

void ARWPlayerController::Move(const FInputActionValue& Value)
{
	// Input Value
	FVector2D MovementVector = Value.Get<FVector2D>(); 

	const FRotator Rotation = this->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	// X, Y Move Value
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// Using Movement Component
	if(MovementVector.X && MovementVector.Y) // 대각 이동 시 속도 조절
	{
		MovementVector.X *= 0.75;
		MovementVector.Y *= 0.75;
	}
	
	if(bIsSneaking)
	{
		MovementVector.X *= 0.2;
		MovementVector.Y *= 0.2;
	}
	else if(!bIsRunning) // Running이 아닐때는 절반의 속도
	{
		MovementVector.X *= 0.5;
		MovementVector.Y *= 0.5;
	}

	PlayerPawn->AddMovementInput(ForwardDirection, MovementVector.X);
	PlayerPawn->AddMovementInput(RightDirection, MovementVector.Y);

	// Focusing 기능 구현
	if(bIsFocusing && FocusingTargetActor != nullptr)
	{
		FVector RotationalVec = FocusingTargetActor->GetActorLocation() - PlayerPawn->GetActorLocation();
		RotationalVec.Z = 0.f;
		RotationalVec.Normalize(); // 방향 벡터를 정규화하여 길이를 1로 만듦

		// 방향 벡터를 회전 값으로 변환
		FRotator NewRotation = FRotationMatrix::MakeFromX(RotationalVec).Rotator();

		PlayerPawn->SetActorRotation(NewRotation);
	}
	
}

void ARWPlayerController::Look(const FInputActionValue& Value)
{
	//if(bIsEnableLook) 우클릭 했을 때 카메라 이동되도록 하려 했는데 보류함
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	this->AddYawInput(LookAxisVector.X);
	this->AddPitchInput(LookAxisVector.Y);
	
	ServerRPCSetLook_Implementation(LookAxisVector);

}
void ARWPlayerController::ServerRPCSetLook_Implementation(FVector2D LookAxisVector)
{
	this->AddYawInput(LookAxisVector.X);
	this->AddPitchInput(LookAxisVector.Y);
}

void ARWPlayerController::EnableLook(const FInputActionValue& Value)
{
	bIsEnableLook = true;
}

void ARWPlayerController::DisableLook(const FInputActionValue& Value)
{
	bIsEnableLook = false;
}

void ARWPlayerController::ControllerJump(const FInputActionValue& Value)
{
	PlayerPawn->Jump();	
}

void ARWPlayerController::ControllerStopJumping(const FInputActionValue& Value)
{
	PlayerPawn->StopJumping();
}

void ARWPlayerController::Run(const FInputActionValue& Value)
{
	bIsRunning = true;
}

void ARWPlayerController::StopRunning(const FInputActionValue& Value)
{
	bIsRunning = false;
}

void ARWPlayerController::Attack(const FInputActionValue& Value)
{
	if(PlayerPawn->bWasJumping || PlayerPawn->CharacterState == ECharacterState::Dead) // 점프중인 경우 공격하지 않음
	{
		return;
	}
	
	//ProcessComboCommand();
	PlayerPawn->Attack();
}

void ARWPlayerController::Sneaking(const FInputActionValue& Value)
{
	bIsSneaking = true;
}

void ARWPlayerController::StopSneaking(const FInputActionValue& Value)
{
	bIsSneaking = false;
}

void ARWPlayerController::Focusing(const FInputActionValue& Value)
{
	
	// Focusing할 Actor를 설정
	if(PlayerPawn->CollisionedPawn)
	{
		FocusingTargetActor = PlayerPawn->CollisionedPawn;
		bIsFocusing = true;
	}
	else if(PlayerPawn->CollisionedItem)
	{
		FocusingTargetActor = PlayerPawn->CollisionedItem;
		bIsFocusing = true;
		UE_LOG(LogTemp, Log, TEXT("PlayerController : Item Pick Up"));
	}
	else
	{
		bIsFocusing = false;
	}
}

void ARWPlayerController::StopFocusing(const FInputActionValue& Value)
{
	bIsFocusing = false;
	// Focusing 되었던 Actor를 해제
	FocusingTargetActor = nullptr;
}



