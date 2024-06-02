// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Riple/RWRifleComponent.h"

#include "EnhancedInputComponent.h"
#include "Interface/RWRifleActionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

URWRifleComponent::URWRifleComponent()
{
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
	FireRange = 10000.0f; // 발사 거리
	BulletDamage = 20.0f;
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
}

void URWRifleComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URWRifleComponent, bIsReadyToShoot);
}

void URWRifleComponent::SetReady()
{
	if(bIsReadyToShoot)
	{
		bIsReadyToShoot = false;
		AbortReady();
		// 카메라 원래 위치로
		RifleActionInterface->StopAiming();
		// 캐릭터 Shoot Ready 해제
		RifleActionInterface->AbortReadyForShoot();

		FName SocketName(TEXT("RifleBackSocket"));
		RifleActionInterface->AttachRifleToSocket(SocketName);
	}
	else
	{
		ReadyToShoot();
		bIsReadyToShoot = true;
		// 카메라 조준 위치로
		RifleActionInterface->StartAiming();
		// 캐릭터 Shoot Ready
		RifleActionInterface->SetReadyForShoot();

		// 클라이언트에서도 
		FName SocketName(TEXT("RifleHandSocket"));
		RifleActionInterface->AttachRifleToSocket(SocketName);
	}
}

void URWRifleComponent::ReadyToShoot_Implementation()
{
	// 사격 준비
	FName SocketName(TEXT("RifleHandSocket"));
	RifleActionInterface->AttachRifleToSocket(SocketName);
}

void URWRifleComponent::AbortReady_Implementation()
{
	// 사격 준비 취소
	FName SocketName(TEXT("RifleBackSocket"));
	RifleActionInterface->AttachRifleToSocket(SocketName);
}

void URWRifleComponent::Fire()
{
	UE_LOG(LogTemp, Log, TEXT("Fire!"));
	
	
	
	PerformLineTrace();
}

void URWRifleComponent::Reload()
{
}

void URWRifleComponent::BindAction()
{
	APawn* OwnerPlayer = Cast<APawn>(GetOwner());
	if(OwnerPlayer)
	{
		APlayerController* PlayerController = Cast<APlayerController>(OwnerPlayer->GetController());
		if (PlayerController)
		{
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
			{
				// Bind PickUp Action
				EnhancedInputComponent->BindAction(ReadyToShootAction, ETriggerEvent::Triggered, this, &URWRifleComponent::SetReady);
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &URWRifleComponent::Fire);
				EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &URWRifleComponent::Reload);
			}
		}
	}
}

void URWRifleComponent::PerformLineTrace()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Log, TEXT("opps!"));
		return;
	}
	APawn* OwnerPawn = Cast<APawn>(Owner);
	APlayerController* OwnerController = Cast<APlayerController>(OwnerPawn->GetController());

	// GetViewportSize
	int32 ViewportSizeX;
	int32 ViewportSizeY;
	OwnerController->GetViewportSize(ViewportSizeX, ViewportSizeY);


	// Get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSizeX / 2.f, ViewportSizeY / 2.f);
	CrosshairLocation.Y -= 50.f;
	
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

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	
	// 라인 트레이스 수행
	bool bHitFirst = GetWorld()->LineTraceSingleByChannel(HitResult, CameraTraceStart, CameraTraceEnd, ECC_Visibility, Params);
	
	if (bHitFirst)
	{
		// 히트 결과 처리
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			// 히트된 액터에 데미지 적용
			//UGameplayStatics::ApplyPointDamage(HitActor, BulletDamage, Rotation.Vector(), HitResult, Owner->GetInstigatorController(), Owner, nullptr);

			// 디버그 라인 그리기 (옵션)
			DrawDebugLine(GetWorld(), CameraTraceStart, HitResult.ImpactPoint, FColor::Red, false, 1, 0, 1);
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10, FColor::Red, false, 1);


			
			// Second LineTrace (Start : Player Rifle)
			// 첫 번째 LineTrace의 피격 지점에 총신으로부터 다시 라인트레이스를 통해 진짜 맞았는지 확인
			FVector RifleTraceStart = RifleActionInterface->GetFireStartLocation();
			FVector RifleTraceEnd = HitResult.ImpactPoint; // 피격 당한 곳

			bool bHitSecond = GetWorld()->LineTraceSingleByChannel(HitResult, RifleTraceStart, RifleTraceEnd, ECC_Visibility, Params);
			if(bHitSecond)
			{
				HitActor = HitResult.GetActor();
				if (HitActor)
				{
					// 여기서 데미지 처리
				}
				
				DrawDebugLine(GetWorld(), RifleTraceStart, HitResult.ImpactPoint, FColor::Green, false, 1, 0, 1);
				DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10, FColor::Green, false, 1);
			}
		}
	}
	else
	{
		// 디버그 라인 그리기 (옵션)
		DrawDebugLine(GetWorld(), CameraTraceStart, CameraTraceEnd, FColor::Blue, false, 1, 0, 1);
	}
}


