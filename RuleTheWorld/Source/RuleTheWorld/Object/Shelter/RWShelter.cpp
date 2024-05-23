// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Shelter/RWShelter.h"
#include "Interface/RWShelterCollisionInterface.h"
#include "Components/BoxComponent.h"

// Sets default values
ARWShelter::ARWShelter()
{
	// Set CollisionBox
	ShelterCollisionBox = CreateDefaultSubobject<UBoxComponent>("ShelterCollisionBox");
	ShelterCollisionBox->InitBoxExtent(FVector(1000.f, 1000.f,500.f));
	ShelterCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ShelterCollisionBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	// Set Overlap Delegate
	ShelterCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ARWShelter::OnOverlapBegin);
	ShelterCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ARWShelter::OnOverlapEnd);
	ShelterCollisionBox->SetupAttachment(RootComponent);
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



