// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/RWAnimalBase.h"

#include "Components/CapsuleComponent.h"
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

	// Spawn시에 Collision이 있어도 생성되도록 설정 
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}

// Called when the game starts or when spawned
void ARWAnimalBase::BeginPlay()
{
	Super::BeginPlay();
	
}

 
void ARWAnimalBase::UpdateWalkSpeed(float WalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}




