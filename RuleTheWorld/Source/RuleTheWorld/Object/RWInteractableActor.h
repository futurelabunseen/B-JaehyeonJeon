// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RWEnums.h"
#include "Interface/RWItemDataInterface.h"
#include "RWInteractableActor.generated.h"


UCLASS()
class RULETHEWORLD_API ARWInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARWInteractableActor();

	// Inventory 구성을 위한 ItemData
	EItemData ItemData = EItemData::None;
	
// DefaultComponent
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TObjectPtr<class UCapsuleComponent> CapsuleComponent;
};
