// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/RWInventoryWidgetInterface.h"
#include "RWInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()

class RULETHEWORLD_API URWInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	URWInventoryWidget(const FObjectInitializer &ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void InventoryCopy();
	
	FInventory CopiedInventory;
};
