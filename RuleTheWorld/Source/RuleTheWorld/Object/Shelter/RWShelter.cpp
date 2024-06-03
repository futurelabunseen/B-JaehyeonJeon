// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Shelter/RWShelter.h"

#include "Components/SphereComponent.h"
#include "Interface/RWShelterCollisionInterface.h"

// Sets default values
ARWShelter::ARWShelter()
{
	// Set CollisionBox
	ShelterCollisionSphere = CreateDefaultSubobject<USphereComponent>("ShelterCollisionSphere");
	ShelterCollisionSphere->InitSphereRadius(1000.f);
	ShelterCollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ShelterCollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	// Set Overlap Delegate
	ShelterCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ARWShelter::OnOverlapBegin);
	ShelterCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ARWShelter::OnOverlapEnd);
	ShelterCollisionSphere->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARWShelter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARWShelter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IRWShelterCollisionInterface* CollisionedPlayer = Cast<IRWShelterCollisionInterface>(OtherActor);
	if(CollisionedPlayer)
	{
		CollisionedPlayer->SetupShelterEntry();
	}
}

void ARWShelter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	IRWShelterCollisionInterface* CollisionedPlayer = Cast<IRWShelterCollisionInterface>(OtherActor);
	if(CollisionedPlayer)
	{
		CollisionedPlayer->SetupShelterExit();
	}
}



