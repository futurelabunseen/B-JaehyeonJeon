// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/RWAnimalWolf.h"

#include "AI/RWAIWolfController.h"

ARWAnimalWolf::ARWAnimalWolf()
{
	// Anim Instance Assign
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/RuleTheWorld/Animal/Animation/Wolf/ABP_Wolf.ABP_Wolf_C"));
	if(AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}
	
	// Assign Mesh Reference 
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> AnimalMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Asset/AnimalVarietyPack/Wolf/Meshes/SK_Wolf.SK_Wolf'"));
	if(AnimalMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(AnimalMeshRef.Object);
	}

}
