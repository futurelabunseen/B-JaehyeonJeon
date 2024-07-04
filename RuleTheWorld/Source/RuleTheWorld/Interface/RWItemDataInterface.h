// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RWEnums.h"
#include "RWItemDataInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URWItemDataInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RULETHEWORLD_API IRWItemDataInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	// Inventory에 시용할 이미지 텍스쳐
	virtual TMap<EItemData, TObjectPtr<class URWItemData>> GetItemDataAssetMap() = 0;


};
