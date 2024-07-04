// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RWEnums.h"
#include "RWInventoryWidgetInterface.generated.h"

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

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URWInventoryWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class RULETHEWORLD_API IRWInventoryWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual FInventory GetInventoryData() = 0;

	virtual void SetUpInventoryWidget(class URWInventoryWidget* InventoryWidget) = 0;

	virtual void UseItem(int32 ItemIndex) = 0;

	virtual void DeleteItem(int32 ItemIndex) = 0;
};
