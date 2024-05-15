// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RWMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API URWMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	URWMainWidget(const FObjectInitializer &ObjectInitializer);


	TObjectPtr<class ARWCharacterPlayer> CharacterPlayer;
	
// Minimap
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Minimap")
	TObjectPtr<class UImage> MinimapImage;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Minimap")
	TObjectPtr<class UMaterial> MinimapMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Minimap")
	TObjectPtr<class UMaterialInstanceDynamic> MinimapMaterialInstance;

	void SetMinimap();
	void SetMinimapMaterial();
	void BindImageFromMinimapMaterial();
	
	FTimerHandle TimerHandle_SetMinimap;
	
	uint8 bIsMinimapCompleted:1;

// Stat Bar (HP, Hunger)
public:
	FORCEINLINE void SetMaxHP(float NewMaxHP) { MaxHP = NewMaxHP; }
	void UpdateHPBar(float NewCurrentHP);

	FORCEINLINE void SetMaxHunger(float NewMaxHunger) { MaxHunger = NewMaxHunger; }
	void UpdateHungerBar(float NewCurrentHunger);
	
	
protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HPProgressBar;
	UPROPERTY()
	float MaxHP;
	
	UPROPERTY()
	TObjectPtr<class UProgressBar> HungerProgressBar;
	UPROPERTY()
	float MaxHunger;
	
};
