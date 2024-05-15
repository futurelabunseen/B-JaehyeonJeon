// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/RWInteractableActor.h"

#include "Components/CapsuleComponent.h"

// Sets default values
ARWInteractableActor::ARWInteractableActor()
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = CapsuleComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
}