// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal/RWAnimalPig.h"

ARWAnimalPig::ARWAnimalPig()
{
	// Anim Instance Assign
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/RuleTheWorld/Animal/Animation/Pig/ABP_Pig.ABP_Pig_C"));
	if(AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}
	
	// Assign Mesh Reference 
	// Assign Mesh Reference 
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> AnimalMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Asset/AnimalVarietyPack/Pig/Meshes/SK_Pig.SK_Pig'"));
	if(AnimalMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(AnimalMeshRef.Object);
	}
}
