// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RWEnums.h"
#include "RWInteractableActor.h"
#include "Interface/RWItemDataInterface.h"
#include "RWItemData.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API URWItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EItemData ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UTexture2D> ItemTexture;
};
