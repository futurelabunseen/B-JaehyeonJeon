// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RWGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARWGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
// Game Time
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	float CurrentTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	float DayProgressPercent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	int32 DayScore;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	int32 CurrentHour;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Day, Meta = (AllowPrivateAccess = "true"))
	int32 CurrentMinute;
	
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
	TArray<TSubclassOf<class ARWAnimalBase>> AnimalClasses;
	
	// 현재 생성된 동물의 수와 '오늘'의 최대 수를 Map으로 저장
	// Map<동물 클래스, Int>
	TMap<TSubclassOf<class ARWAnimalBase>, int32> AnimalMaxNumMap;
	TMap<TSubclassOf<class ARWAnimalBase>, int32> AnimalCurrentNumMap;
	

	// 날짜 변경 시 변경된 동물의 수에 맞게 스폰
	void DayChangeSpawnAnimals();
	
	void UpdateMaxWolfNum();

	
public:
	// 사냥 시 현재 수를 줄여줌
	void DecreaseCurrentAnimalNum(class ARWAnimalBase*);
};

