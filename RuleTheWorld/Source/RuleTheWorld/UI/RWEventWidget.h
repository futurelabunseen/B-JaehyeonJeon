// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RWEventWidget.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API URWEventWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> DayEventImage;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> NightEventImage;

	UFUNCTION()
	void SetDayEventImagVisible(uint8 bVisible);
	UFUNCTION()
	void SetNightEventImagVisible(uint8 bVisible);
};
