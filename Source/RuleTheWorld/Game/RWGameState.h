// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RWGameState.generated.h"

/**
 * 
 */

UCLASS()
class RULETHEWORLD_API ARWGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ARWGameState();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	TObjectPtr<class ARWGameMode> GameMode;
// Network
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/*
	UFUNCTION(Server, Reliable)
	void ServerRequestTimeVariable();
	

	
	void RequestTimeVariableFromServer();
	*/

	UPROPERTY(BlueprintReadOnly, Category = "Time")
	FTimerHandle TimerHandle_RequestGameTime;

	// Time Variable
	void UpdateTime(float DeltaSeconds);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Time")
	int32 DayScore;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Time")
	float CurrentTime;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = "Time")
	float DayProgressPercent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	int32 CurrentHour;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	int32 CurrentMinute;

public:
	float GetClientCurrentTime() const;
	float GetClientDayProgressPercent() const;
	int32 GetClientDayScore() const;
};
