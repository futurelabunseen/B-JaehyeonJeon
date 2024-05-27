// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/RWCollisionedItemInterface.h"
#include "Interface/RWInventoryWidgetInterface.h"
#include "RWInventoryComponent.generated.h"



DECLARE_MULTICAST_DELEGATE(FOnRepInventoryDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RULETHEWORLD_API URWInventoryComponent : public UActorComponent, public IRWInventoryWidgetInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URWInventoryComponent();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	TScriptInterface<IRWCollisionedItemInterface> CharacterInterface;

protected:
// RWInventoryWidgetInterface
	// Inventory는 <무슨 아이템, 몇 개>의 형태로 저장됨
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_CopiedInventoryToWidget, EditAnywhere, Category = "Inventory")
	FInventory Inventory;
	
	virtual FInventory GetInventoryData() override;
	virtual void SetUpInventoryWidget(URWInventoryWidget* InventoryWidget) override;
	
	FOnRepInventoryDelegate OnRepInventory;
	
	UFUNCTION()
	void OnRep_CopiedInventoryToWidget();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> InventoryWidgetOnScreenAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> PickUpAction;
	
	void PickUp();
	UFUNCTION(Server, WithValidation, Reliable)
	void ServerRPCGetItem(EItemData ItemData);

	void UseItem();
	void DeleteItem();

	void BindAction();
	void InitializeInterface();

	int32 GetItemIndex(EItemData ItemData);
	void AddInventoryItemNums(int32 ItemIndex);

	
// Inventory UI
	
	// BP에서 지정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Inventory, Meta = (AllowPrivateAccess = "True"))
	TSubclassOf<UUserWidget> InventoryWidgetClass;
	
	void InventoryWidgetInstancing();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Inventory, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<UUserWidget> InventoryWidgetInstance;
	
	void SetInventoryWidgetVisibility();

};


