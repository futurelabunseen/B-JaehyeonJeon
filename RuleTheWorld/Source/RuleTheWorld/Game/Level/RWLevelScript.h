// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "RWLevelScript.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWLevelScript : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
	ARWLevelScript();

	virtual void PostNetInit() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameState")
	TObjectPtr<class ARWGameState> GameState;
	
// Widget
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> WidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UUserWidget> MainWidget;

	// Client Only
	UFUNCTION(Client, Reliable)
	void AddMainWidget();
	
// Sky
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sky", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDirectionalLightComponent> SunLight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sky", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDirectionalLightComponent> MoonLight;
	
	// Client Only
	UFUNCTION(Client, Reliable)
	void UpdateSunLightRotation();
	
};
