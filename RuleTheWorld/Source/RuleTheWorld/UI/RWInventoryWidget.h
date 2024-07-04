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

// Item Image
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

// Item Num
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNumText0;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNumText1;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNumText2;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNumText3;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNumText4;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNumText5;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNumText6;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNumText7;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNumText8;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> ItemNumText9;

	TArray<TObjectPtr<class UTextBlock>> ItemNumTexts;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetItemText(UTextBlock* TextBlock, const uint8& ItemNum);

// Item use Button
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> ItemUseBtn0;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> ItemUseBtn1;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> ItemUseBtn2;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> ItemUseBtn3;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> ItemUseBtn4;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> ItemUseBtn5;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> ItemUseBtn6;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> ItemUseBtn7;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> ItemUseBtn8;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> ItemUseBtn9;
protected:
	void BindItemUseBtn();
	
	UFUNCTION()
	void UseItemHandler0();
	UFUNCTION()
	void UseItemHandler1();
	UFUNCTION()
	void UseItemHandler2();
	UFUNCTION()
	void UseItemHandler3();
	UFUNCTION()
	void UseItemHandler4();
	UFUNCTION()
	void UseItemHandler5();
	UFUNCTION()
	void UseItemHandler6();
	UFUNCTION()
	void UseItemHandler7();
	UFUNCTION()
	void UseItemHandler8();
	UFUNCTION()
	void UseItemHandler9();
};
