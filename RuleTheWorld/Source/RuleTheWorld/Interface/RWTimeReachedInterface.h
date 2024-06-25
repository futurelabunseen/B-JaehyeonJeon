// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RWTimeReachedInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeReached);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URWTimeReachedInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RULETHEWORLD_API IRWTimeReachedInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FOnTimeReached& GetOnTimeReachedMorning() = 0;
	virtual FOnTimeReached& GetOnTimeReachedNight() = 0; 
};
