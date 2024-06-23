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
	
}

void URWRifleComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URWRifleComponent, bIsReadyToShoot);
	DOREPLIFETIME(URWRifleComponent, bIsAiming);
	DOREPLIFETIME(URWRifleComponent, BulletNum);
	DOREPLIFETIME(URWRifleComponent, LoadedBullet);
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

void URWRifleComponent::NetMulticastRPCShootFire_Implementation(FHitResult CameraHitResult, FHitResult RifleHitResult)
{
	OwnerPlayer->PlayAnimMontage(RifleFireMontage, 3);
	
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, OwnerPlayer->GetActorLocation());
	}

	// Rifle HitResult로 하는게 엄밀히 맞지만.... 현재 구조로는 Rifle HitResult가 존재하는 경우는 CameraHitResult와 같다.
	// 추후 수정이 필요할 듯
	if (CameraHitResult.bBlockingHit) 
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraEffect, CameraHitResult.Location, FRotator::ZeroRotator);
		UE_LOG(LogTemp, Log, TEXT("%f, %f, %f"), CameraHitResult.Location.X, CameraHitResult.Location.Y, CameraHitResult.Location.Z);
	}

	
	// 동물이 맞은 경우 점프
	ARWAnimalBase* HitAnimal = Cast<ARWAnimalBase>(RifleHitResult.GetActor());
	if(HitAnimal)
	{
		HitAnimal->Jump();
	}
				
}

void URWRifleComponent::ServerRPCPerformLineTrace_Implementation(FVector CameraTraceStart, FVector CameraTraceEnd)
{
	
	FHitResult CameraHitResult;
	FHitResult RifleHitResult;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerPlayer);
	
	// 라인 트레이스 수행
	bool bHitRifle = GetWorld()->LineTraceSingleByChannel(CameraHitResult, CameraTraceStart, CameraTraceEnd, ECC_Visibility, Params);
	DrawDebugLine(GetWorld(), CameraTraceStart, CameraTraceEnd, FColor::Blue, false, 1, 0, 1);
	
	if (bHitRifle)
	{
		// 히트 결과 처리
		AActor* HitActor = CameraHitResult.GetActor();
		if (HitActor)
		{
			// 디버그 라인 그리기 (옵션)
			DrawDebugLine(GetWorld(), CameraTraceStart, CameraHitResult.ImpactPoint, FColor::Red, false, 1, 0, 1);
			DrawDebugPoint(GetWorld(), CameraHitResult.ImpactPoint, 10, FColor::Red, false, 1);

			// Second LineTrace (Start : Player Rifle)
			// 첫 번째 LineTrace의 피격 지점에 총신으로부터 다시 라인트레이스를 통해 진짜 맞았는지 확인
			FVector RifleTraceStart = RifleActionInterface->GetFireStartLocation();
			FVector RifleTraceEnd = CameraHitResult.ImpactPoint; // 피격 당한 곳

			
			bool bHitSecond = GetWorld()->LineTraceSingleByChannel(RifleHitResult, RifleTraceStart, RifleTraceEnd, ECC_Pawn, Params);
			if(bHitSecond)
			{
				HitActor = RifleHitResult.GetActor();
				if (HitActor)
				{
					// 여기서 데미지 처리
				}

				/*DrawDebugLine(GetWorld(), RifleTraceStart, RifleHitResult.ImpactPoint, FColor::Green, false, 1, 0, 1);
				DrawDebugPoint(GetWorld(), RifleHitResult.ImpactPoint, 10, FColor::Green, false, 1);
				*/

				UE_LOG(LogTemp,Log, TEXT("Hit Result : %s"), *RifleHitResult.GetActor()->GetName());

				ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
				if(HitCharacter)
				{
					float AttackDamage = 100.0f;
					FDamageEvent DamageEvent;
					HitCharacter->TakeDamage(AttackDamage, DamageEvent, OwnerPlayer->GetController(), OwnerPlayer);
				}
				
				
			}
		}
	}
	else
	{
		// 디버그 라인 그리기 (옵션)
		DrawDebugLine(GetWorld(), CameraTraceStart, CameraTraceEnd, FColor::Blue, false, 1, 0, 1);
	}


	// 총격 애니메이션 및 효과
	NetMulticastRPCShootFire(CameraHitResult, RifleHitResult);
}

void URWRifleComponent::SetReady()
{
	if(bIsReadyToShoot) 
	{
		// 준비 취소
		
		bIsReadyToShoot = false;
		bIsAiming = false;
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
	OwnerPlayer->GetMesh()->SetRelativeRotation(FRotator(0.0f, -60.f,0.0f));
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
	OwnerPlayer->GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.f,0.0f));
	ServerRPCCompleteAiming();
}

void URWRifleComponent::ServerRPCStartAiming_Implementation()
{
	SetAnimAiming(true);
	OwnerPlayer->bUseControllerRotationYaw = true;
	OwnerPlayer->GetMesh()->SetRelativeRotation(FRotator(0.0f, -60.f,0.0f));
	NetMulticastRPCSetAiming(true);
}

void URWRifleComponent::ServerRPCCompleteAiming_Implementation()
{
	SetAnimAiming(false);
	OwnerPlayer->bUseControllerRotationYaw = false;
	OwnerPlayer->GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.f,0.0f));
	NetMulticastRPCSetAiming(false);
}


void URWRifleComponent::NetMulticastRPCSetAiming_Implementation(uint8 _bIsAiming)
{
	SetAnimAiming(_bIsAiming);
}

void URWRifleComponent::Fire()
{
	if(!bIsAiming) // 조준하고 있지 않은 경우에 총 쏘지 못하게 함
	{
		return;
	}

	// GetViewportSize
	int32 ViewportSizeX;
	int32 ViewportSizeY;
	APlayerController* OwnerController = Cast<APlayerController>(OwnerPlayer->GetController());
	OwnerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// Get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSizeX / 2.f, ViewportSizeY / 2.f);
	CrosshairLocation.Y ;
	
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// Get world position and direction of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
		(UGameplayStatics::GetPlayerController(this, 0),// Player의 controller
		CrosshairLocation,  // 2D 상의 Crosshair의 위치
		CrosshairWorldPosition,   // 3D상의 Crosshair 계산 결과를 얻을 변수
		CrosshairWorldDirection); // 3D상의 Crosshair 의 방향

	// First LineTrace (Start : CrossHair)
	FVector CameraTraceStart = CrosshairWorldPosition;
	//FVector Start = RifleActionInterface->GetFireStartLocation();
	FVector CameraTraceEnd = CameraTraceStart + (CrosshairWorldDirection * FireRange);

	// 총격에 대해 서버 RPC를 보냄
	ServerRPCPerformLineTrace(CameraTraceStart, CameraTraceEnd);
	
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


