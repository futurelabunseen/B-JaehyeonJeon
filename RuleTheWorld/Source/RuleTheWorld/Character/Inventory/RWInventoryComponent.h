// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RWEnums.h"
#include "RWInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventory // 두 배열이 같이 리플리케이션 되었으면 해서
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<TEnumAsByte<EItemData>> InventoryItemSubjects;
	UPROPERTY(BlueprintReadOnly)
	// 음수가 되지 않으니 uint8로 선언
	TArray<uint8> InventoryItemNums;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RULETHEWORLD_API URWInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URWInventoryComponent();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	TObjectPtr<class APlayerController> PlayerController;
	
	// Inventory는 <무슨 아이템, 몇 개>의 형태로 저장됨
	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Inventory")
	FInventory Inventory;

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> PickUpAction;
	
	void PickUp();
	UFUNCTION(Server, WithValidation, Reliable)
	void ServerRPCGetItem(EItemData ItemData);

	void UseItem();
	void DeleteItem();

	int32 GetItemIndex(EItemData ItemData);
	void AddInventoryItemNums(int32 ItemIndex);
};
