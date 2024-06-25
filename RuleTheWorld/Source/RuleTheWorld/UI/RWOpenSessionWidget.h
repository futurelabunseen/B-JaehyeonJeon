// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RWOpenSessionWidget.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API URWOpenSessionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UButton> CreateServerBtn;
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UButton> JoinServerBtn;
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UButton> QuitGameBtn;
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> IPAddressTextBox;
	UFUNCTION()
	void CreateServerHandler();
	UFUNCTION()
	void JoinServerHandler();
	UFUNCTION()
	void QuitGameHandler();
};
