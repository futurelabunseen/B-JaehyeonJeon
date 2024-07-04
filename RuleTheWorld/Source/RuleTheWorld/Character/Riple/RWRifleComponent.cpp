// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Riple/RWRifleComponent.h"

#include "EnhancedInputComponent.h"
#include "Animal/RWAnimalBase.h"
#include "Animation/RWAnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Interface/RWRifleActionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"

constexpr uint8 INITIAL_BULLET = 100;
constexpr float FIRE_RANGE = 10000.0f; // 발사 거리
constexpr float BULLET_DAMAGE = 20.0f;
constexpr uint8 LOADED_BULLET_NUM = 5;

URWRifleComponent::URWRifleComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> AimingActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_Aiming.IA_Aiming'"));
	if(nullptr != AimingActionRef.Object)
	{
		AimingAction = AimingActionRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> ReadyToShootActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_ReadyToShoot.IA_ReadyToShoot'"));
	if(nullptr != ReadyToShootActionRef.Object)
	{
		ReadyToShootAction = ReadyToShootActionRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> ReloadActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_Reload.IA_Reload'"));
	if(nullptr != ReloadActionRef.Object)
	{
		ReloadAction = ReloadActionRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> FireActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_Fire.IA_Fire'"));
	if(nullptr != FireActionRef.Object)
	{
		FireAction = FireActionRef.Object;
	}
	
	SetIsReplicatedByDefault(true);
	
	// 초기 값 설정
	FireRange = FIRE_RANGE; // 발사 거리
	BulletDamage = BULLET_DAMAGE;

	BulletNum = INITIAL_BULLET;
	LoadedBullet = 0;
}


// Called when the game starts
void URWRifleComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(true);
	
	BindAction();
	
	// Interface 설정
	IRWRifleActionInterface* RI = Cast<IRWRifleActionInterface>(GetOwner());
	RifleActionInterface.SetInterface(RI);
	RifleActionInterface.SetObject(GetOwner());

	// 초기 소켓 부착
	FName SocketName(TEXT("RifleBackSocket"));
	RifleActionInterface->AttachRifleToSocket(SocketName);

	if (CrossHairWidget != nullptr)
	{
		APlayerController* PlayerController = Cast<APlayerController>(OwnerPlayer->GetController());
		CrossHairWidgetInstance = CreateWidget<UUserWidget>(PlayerController, CrossHairWidget);

		if(CrossHairWidgetInstance)
		{
			CrossHairWidgetInstance->AddToViewport();
			CrossHairWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	// 현재 시간을 기반으로 시드 생성
	int32 CurrentRealTime = FDateTime::Now().GetSecond() * FDateTime::Now().GetMillisecond();
	// Rifle의 식별 ID 할당
	RifleID = CurrentRealTime;//RandomStream.RandRange(INT32_MIN,INT32_MAX);
}

void URWRifleComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 리타기팅 과정에서 틀어진 애니메이션을 보강하기 위해...
	if (bIsAiming)
	{
		AController* Controller = OwnerPlayer->GetController();
		if(Controller)
		{
			ControlRotation = OwnerPlayer->GetController()->GetControlRotation();
		}
		
		OwnerPlayer->GetMesh()->SetRelativeRotation(AimingRotation);
		URWAnimInstance* RWAnimInstance = RifleActionInterface->GetAnimInstance();;
		RWAnimInstance->ControlRotation = ControlRotation;

	}
	else
	{
		OwnerPlayer->GetMesh()->SetRelativeRotation(DefaultRotation);
	}
}


void URWRifleComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URWRifleComponent, bIsReadyToShoot);
	DOREPLIFETIME(URWRifleComponent, bIsAiming);
	DOREPLIFETIME(URWRifleComponent, BulletNum);
	DOREPLIFETIME(URWRifleComponent, LoadedBullet);
	DOREPLIFETIME(URWRifleComponent, ControlRotation);
}



void URWRifleComponent::Reload()
{
	if(!bIsReadyToShoot) // 총을 들고 있지 않은 경우
	{
		return;
	}
	
	if(LoadedBullet > 0 || BulletNum < LOADED_BULLET_NUM) //  
	{
		return;
	}
	
	ServerRPCReload();
}


void URWRifleComponent::ServerRPCReload_Implementation()
{
	BulletNum = FMath::Clamp(BulletNum - LOADED_BULLET_NUM, 0, 100);
	LoadedBullet = LOADED_BULLET_NUM;
	UE_LOG(LogTemp, Log, TEXT("%d %d"), BulletNum, LoadedBullet);
	NetMulticastRPCReload();
}

void URWRifleComponent::NetMulticastRPCReload_Implementation()
{
	// 애니메이션 재생
	if(ReloadMontage)
	{
		OwnerPlayer->PlayAnimMontage(ReloadMontage);
	}
}

void URWRifleComponent::BindAction()
{
	OwnerPlayer = Cast<ACharacter>(GetOwner());
	if(OwnerPlayer)
	{
		APlayerController* PlayerController = Cast<APlayerController>(OwnerPlayer->GetController());
		if (PlayerController)
		{
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
			{
				// Bind PickUp Action
				EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Triggered, this, &URWRifleComponent::StartAiming);
				EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &URWRifleComponent::StopAiming);
   
				EnhancedInputComponent->BindAction(ReadyToShootAction, ETriggerEvent::Triggered, this, &URWRifleComponent::SetReady);
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &URWRifleComponent::Fire);
				EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &URWRifleComponent::Reload);
			}
		}
	}
}

void URWRifleComponent::NetMulticastRPCShootFire_Implementation(int32 InputRifleID, FHitResult RifleHitResult)
{
	// Client가 사격을 실행한 클라이언트일 경우 이미 애니메이션과 사운드가 출력되었음
	if(RifleID != InputRifleID) // 아닌 경우에 출력
	{
		OwnerPlayer->PlayAnimMontage(RifleFireMontage, 3);
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, OwnerPlayer->GetActorLocation());
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("qwe %d %d"), RifleID, InputRifleID);
	

	// Rifle HitResult의 피격위치에 나이아가라 이펙트 생성
	if (RifleHitResult.bBlockingHit) 
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraEffect, RifleHitResult.Location, FRotator::ZeroRotator);
		UE_LOG(LogTemp, Log, TEXT("Rifle : %f, %f, %f"), RifleHitResult.Location.X, RifleHitResult.Location.Y, RifleHitResult.Location.Z);
	}
	
	/*// 동물이 맞은 경우 점프
	ARWAnimalBase* HitAnimal = Cast<ARWAnimalBase>(RifleHitResult.GetActor());
	if(HitAnimal)
	{
		HitAnimal->Jump();
	}*/
				
}

void URWRifleComponent::ServerRPCPerformLineTrace_Implementation(int InputRifleID, FVector CameraTraceStart, FVector CameraTraceEnd)
{
	
	FHitResult CameraHitResult;
	FHitResult RifleHitResult;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerPlayer);
	
	// 라인 트레이스 수행
	bool bHitRifle = GetWorld()->LineTraceSingleByChannel(CameraHitResult, CameraTraceStart, CameraTraceEnd, ECC_Visibility, Params);
	//DrawDebugLine(GetWorld(), CameraTraceStart, CameraTraceEnd, FColor::Blue, false, 1, 0, 1);
	
	if (bHitRifle)
	{
		// 히트 결과 처리
		AActor* HitActor = CameraHitResult.GetActor();
		if (HitActor)
		{
			/*// 디버그 라인 그리기 (옵션)
			DrawDebugLine(GetWorld(), CameraTraceStart, CameraHitResult.ImpactPoint, FColor::Red, false, 1, 0, 1);
			DrawDebugPoint(GetWorld(), CameraHitResult.ImpactPoint, 10, FColor::Red, false, 1);
			*/

			// Second LineTrace (Start : Player Rifle)
			// 첫 번째 LineTrace의 피격 지점에 총신으로부터 다시 라인트레이스를 통해 진짜 맞았는지 확인
			FVector RifleTraceStart = RifleActionInterface->GetFireStartLocation();
			FVector RifleTraceEnd = CameraHitResult.ImpactPoint; // 피격 당한 곳
			
			bool bHitSecond = GetWorld()->LineTraceSingleByChannel(RifleHitResult, RifleTraceStart, RifleTraceEnd, ECC_PhysicsBody, Params);
			if(bHitSecond)
			{
				HitActor = RifleHitResult.GetActor();
				UE_LOG(LogTemp,Log, TEXT("Hit Result : %s"), *RifleHitResult.GetActor()->GetName());

				ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
				if(HitCharacter)
				{
					float AttackDamage = 30.0f;
					FDamageEvent DamageEvent;
					HitCharacter->TakeDamage(AttackDamage, DamageEvent, OwnerPlayer->GetController(), OwnerPlayer);
				}
			}
		}
	}
	
	// 총격 애니메이션 및 효과
	NetMulticastRPCShootFire(InputRifleID,RifleHitResult);
}

void URWRifleComponent::SetReady()
{
	if(bIsReadyToShoot) 
	{
		if(bIsAiming) // Aim 중이었으면 취소하는 기능 <- Ready 취소보다 먼저 수행되어야 함. 
		{
			StopAiming();
			bIsAiming = false;
		}
		
		// 준비 취소
		bIsReadyToShoot = false;

		ServerRPCAbortReady();
		// 카메라 원래 위치로
		RifleActionInterface->StopAiming();
		
	
		
		SetAnimReadyToShoot(false);
	}
	else
	{
		bIsReadyToShoot = true;
		ServerRPCReadyToShoot();
		
		// 카메라 조준 위치로
		RifleActionInterface->StartAiming();
		
		SetAnimReadyToShoot(true);
	}
}

void URWRifleComponent::ServerRPCReadyToShoot_Implementation()
{
	SetAnimReadyToShoot(true);

	NetMulticastRPCAnimReadyToShoot(true);
}

void URWRifleComponent::ServerRPCAbortReady_Implementation()
{
	SetAnimReadyToShoot(false);

	NetMulticastRPCAnimReadyToShoot(false);
}

void URWRifleComponent::StartAiming()
{
	if(!bIsReadyToShoot || OwnerPlayer->JumpCurrentCount > 0) // 총을 들지 않았거나 점프 중일때는 수행하지 않음
	{
		return;
	}
	if(CrossHairWidgetInstance)
	{
		CrossHairWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
	bIsAiming = true;
	OwnerPlayer->bUseControllerRotationYaw = true;
	ServerRPCStartAiming();
}

void URWRifleComponent::StopAiming()
{
	if(!bIsReadyToShoot)
	{
		return;
	}

	if(CrossHairWidgetInstance)
	{
		CrossHairWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}
	bIsAiming = false;
	OwnerPlayer->bUseControllerRotationYaw = false;
	ServerRPCCompleteAiming();
}

void URWRifleComponent::ServerRPCStartAiming_Implementation()
{
	SetAnimAiming(true);
	OwnerPlayer->bUseControllerRotationYaw = true;

	bIsAiming = true;
	NetMulticastRPCSetAiming(true);
}

void URWRifleComponent::ServerRPCCompleteAiming_Implementation()
{
	SetAnimAiming(false);
	OwnerPlayer->bUseControllerRotationYaw = false;
	bIsAiming = false;
	NetMulticastRPCSetAiming(false);
}


void URWRifleComponent::NetMulticastRPCSetAiming_Implementation(uint8 _bIsAiming)
{
	bIsAiming = _bIsAiming;
	
	SetAnimAiming(_bIsAiming);
}

void URWRifleComponent::Fire()
{
	if(!bIsAiming) // 조준하고 있지 않은 경우에 총 쏘지 못하게 함
	{
		return;
	}
	// Viewport 크기 
	int32 ViewportSizeX;
	int32 ViewportSizeY;
	APlayerController* OwnerController = Cast<APlayerController>(OwnerPlayer->GetController());
	OwnerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// CrossHari의 Screen Space 좌표
	FVector2D CrosshairLocation(ViewportSizeX / 2.f, ViewportSizeY / 2.f);
	CrosshairLocation.Y ;
	
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// crosshair의 위치를 바탕으로 Screen Space에서 World Space로 Deproject
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
		(UGameplayStatics::GetPlayerController(this, 0),// Player의 controller
		CrosshairLocation,  // 2D 상의 Crosshair의 위치
		CrosshairWorldPosition,   // World Space에서의 좌표
		CrosshairWorldDirection  // World Space에서의 방향
		); 

	// First LineTrace (Start : CrossHair)
	FVector CameraTraceStart = CrosshairWorldPosition;
	//FVector Start = RifleActionInterface->GetFireStartLocation();
	FVector CameraTraceEnd = CameraTraceStart + (CrosshairWorldDirection * FireRange);

	// 총격에 대해 서버 RPC를 보냄
	ServerRPCPerformLineTrace(RifleID, CameraTraceStart, CameraTraceEnd);

	// 총격 애니메이션 출력
	OwnerPlayer->PlayAnimMontage(RifleFireMontage, 3);

	// 총격 사운드 출력
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, OwnerPlayer->GetActorLocation());
	}

}

void URWRifleComponent::NetMulticastRPCAnimReadyToShoot_Implementation(uint8 _bIsReadyToShoot)
{
	SetAnimReadyToShoot(_bIsReadyToShoot);
	FName SocketName;
	if(_bIsReadyToShoot)
	{
		// 사격 준비
		SocketName = (TEXT("RifleHandSocket"));
		RifleActionInterface->AttachRifleToSocket(SocketName);
	}
	else
	{
		SocketName = (TEXT("RifleBackSocket"));
		RifleActionInterface->AttachRifleToSocket(SocketName);
	}
	RifleActionInterface->AttachRifleToSocket(SocketName);
}

void URWRifleComponent::SetAnimReadyToShoot(uint8 _bIsReadyForShoot)
{
	RifleActionInterface->GetAnimInstance()->bIsRifleSet = _bIsReadyForShoot;

	if(!_bIsReadyForShoot) // Ready를 푸는 상황에서 Aiming이 되어있다면 해제
	{
		RifleActionInterface->GetAnimInstance()->bIsAiming = false;
	}
}

void URWRifleComponent::SetAnimAiming(uint8 _bIsAiming)
{
	URWAnimInstance* RWAnimInstance = RifleActionInterface->GetAnimInstance();
	if(RWAnimInstance)
	{
		RWAnimInstance->bIsAiming = _bIsAiming;
	}
}


