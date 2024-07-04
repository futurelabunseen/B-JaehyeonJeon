// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RWEnums.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/RWTimeReachedInterface.h"
#include "RWGameMode.generated.h"
/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWGameMode : public AGameModeBase, public IRWTimeReachedInterface
{
	GENERATED_BODY()

public:
	ARWGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	TObjectPtr<class ARWGameState> GameState;
	
// Game Time
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	float CurrentTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	float DayProgressPercent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	int32 DayScore;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	int32 CurrentHour = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	int32 CurrentMinute = 0;
	
	void UpdateTime(float DeltaSeconds);

public:
	float GetCurrentTime() const;
	float GetDayProgressPercent() const;
	int32 GetDayScore() const;

	// 날짜가 바뀔 때 수행할 task들
	void DayChange();

// Spawn Animal And Plant
private:
	// Animal Class
	UPROPERTY(EditAnywhere, Category = Animal)
	TSubclassOf<class ARWAnimalBase> WolfClass;
	UPROPERTY(EditAnywhere, Category = Animal)
	TSubclassOf<class ARWAnimalBase> PigClass;
	UPROPERTY(EditAnywhere, Category = Animal)
	TSubclassOf<class ARWAnimalBase> FoxClass;
	TMap<EAnimalData, TSubclassOf<class ARWAnimalBase>> AnimalClassMap;
	
	// 현재 생성된 동물의 수와 '오늘'의 최대 수를 Map으로 저장
	// Map<동물 클래스, <Max, Current>>
	TMap<EAnimalData, TArray<int32>> AnimalNumMap;

	void HandleActivateAnimal();

	// 날짜 변경 시 변경된 동물의 수에 맞게 스폰
	void DayChangeSpawnAnimals();

	// 밤이 되었을 때, 일정 주기로 늑대를 스폰
	void NightWolfSpawn();

	UFUNCTION()
	void UpdateMaxWolfNum();
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTimeReached OnTimeReachedMorning; // 낮이 되었음을 알리는 Delegate
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTimeReached OnTimeReachedNight; // 방이 되었음을 알리는 Delegate

	virtual FOnTimeReached& GetOnTimeReachedMorning() override;
	virtual FOnTimeReached& GetOnTimeReachedNight() override; 
};

