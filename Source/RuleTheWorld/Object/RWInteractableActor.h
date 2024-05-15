// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RWInteractableActor.generated.h"

UCLASS()
class RULETHEWORLD_API ARWInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARWInteractableActor();


// DefaultComponent
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TObjectPtr<class UStaticMeshComponent> MeshComponent;
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TObjectPtr<class UCapsuleComponent> CapsuleComponent;
};
