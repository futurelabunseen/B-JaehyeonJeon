// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RW_SessionGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARW_SessionGameMode : public AGameModeBase
{
	GENERATED_BODY()

	void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI", meta= (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> SessionWidgetClass;

	TObjectPtr<class UUserWidget> SessionWidget;

public:
	void CreateServer();
	void JoinServer(FString IP);
	void QuitGame();
};
