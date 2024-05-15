// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RWPlayerInventory.generated.h"

UCLASS()
class RULETHEWORLD_API ARWPlayerInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	ARWPlayerInventory();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	TArray<TObjectPtr<class ARWInteractableActor>> Inventory;

	UFUNCTION()
	void GetItem(class ARWInteractableActor* Item);

	UFUNCTION()
	void UseItem();

	UFUNCTION()
	void DeleteItem();
	
};