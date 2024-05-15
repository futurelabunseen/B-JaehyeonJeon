// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/RWAnimalDeer.h"

ARWAnimalDeer::ARWAnimalDeer()
{
	// Assign Mesh Reference 
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> AnimalMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Asset/AnimalVarietyPack/DeerStagAndDoe/Meshes/SK_DeerDoe.SK_DeerDoe'"));
	if(AnimalMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(AnimalMeshRef.Object);
	}
}
