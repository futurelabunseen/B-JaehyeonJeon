// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RWAnimalBase.generated.h"

UCLASS()
class RULETHEWORLD_API ARWAnimalBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARWAnimalBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateWalkSpeed(float WalkSpeed);
};
