// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RWEnums.h"
#include "RWPlayerInventory.generated.h"

UCLASS()
class RULETHEWORLD_API ARWPlayerInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	ARWPlayerInventory();

	// Inventory는 <무슨 아이템, 몇 개>의 형태로 저장됨
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	TMap<TEnumAsByte<EItemData>, int32> Inventory;

	UFUNCTION()
	void GetItem(class ARWInteractableActor* Item);

	UFUNCTION()
	void UseItem();

	UFUNCTION()
	void DeleteItem();
	
};