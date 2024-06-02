// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RWEnums.h"
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
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Inventory)
	FInventory CopiedInventory;

	TObjectPtr<UActorComponent> InventoryComponent;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> InventoryImage0;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> InventoryImage1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> InventoryImage2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> InventoryImage3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> InventoryImage4;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> InventoryImage5;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> InventoryImage6;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> InventoryImage7;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> InventoryImage8;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> InventoryImage9;

	TArray<TObjectPtr<class UImage>> InventoryImages;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetImageTexture(UImage* ImageWidget, UTexture2D* NewTexture);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void BindTexturesFromInterface();
	
};
