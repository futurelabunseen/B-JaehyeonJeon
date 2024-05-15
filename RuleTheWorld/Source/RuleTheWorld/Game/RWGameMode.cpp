// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/RWGameMode.h"

#include "RWGameState.h"

#include "Animal/RWAnimalBase.h"


#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Player/RWPlayerController.h"

constexpr float START_TIME = 120.f; // Game Start 06:00 
constexpr float ONE_DAY = 480.f; // 8min(Real Time) = 1Day / 1min = 3Hours / 20sec = 1Hour
constexpr int ONE_HOUR = 20;

ARWGameMode::ARWGameMode()
{
	//DefaultPawnClass <- 해야 함
	PrimaryActorTick.bCanEverTick = true;

	//static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/RuleTheWorld.RWCharacterPlayer"));
	static  ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/RuleTheWorld/Character/BP_RWCharacterPlayer.BP_RWCharacterPlayer_C"));
	if(DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	
	// Set PlayerController 
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Game/RuleTheWorld/Character/BP_RWPlayerController.BP_RWPlayerController_C"));
	if(PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	// Set GameState
	static ConstructorHelpers::FClassFinder<AGameStateBase> GameStateClassRef(TEXT("/Script/RuleTheWorld.RWGameState"));
    if(GameStateClassRef.Class)
    {
    	GameStateClass = GameStateClassRef.Class;
    }

	// Set PlayerState
	static ConstructorHelpers::FClassFinder<APlayerState> PlayerStateClassRef(TEXT("/Script/RuleTheWorld.RWPlayerState"));
	if(PlayerStateClassRef.Class)
	{
		PlayerStateClass = PlayerStateClassRef.Class;
	}

	// Set Animal Class
	static ConstructorHelpers::FClassFinder<ARWAnimalBase> WolfClassRef(TEXT("/Game/RuleTheWorld/Animal/BP_Wolf.BP_Wolf_C"));
	if(WolfClassRef.Class)
	{
		WolfClass = WolfClassRef.Class;
	}
	AnimalClasses.Add(WolfClass);
		
	static ConstructorHelpers::FClassFinder<ARWAnimalBase> PigClassRef(TEXT("/Game/RuleTheWorld/Animal/BP_Pig.BP_Pig_C"));
	if(PigClassRef.Class)
	{
		PigClass = PigClassRef.Class;
	}
	AnimalClasses.Add(PigClass);
	
	static ConstructorHelpers::FClassFinder<ARWAnimalBase> FoxClassRef(TEXT("/Game/RuleTheWorld/Animal/BP_Fox.BP_Fox_C"));
	if(FoxClassRef.Class)
	{
		FoxClass = FoxClassRef.Class;
	}
	AnimalClasses.Add(FoxClass);
	
	// Day
	CurrentTime = START_TIME;
	DayScore = 1;
	DayProgressPercent = START_TIME / ONE_DAY;

	// Animal
	// Set Initial Value
	AnimalMaxNumMap.Add(WolfClass, 3);
	AnimalMaxNumMap.Add(PigClass, 10);
	AnimalMaxNumMap.Add(FoxClass, 1);

	AnimalCurrentNumMap.Add(WolfClass, 0);
	AnimalCurrentNumMap.Add(PigClass, 0);
	AnimalCurrentNumMap.Add(FoxClass, 0);
}

void ARWGameMode::BeginPlay()
{
	Super::BeginPlay();

	// First Day Setting
	DayChangeSpawnAnimals();
}

void ARWGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateTime(DeltaSeconds);
}

void ARWGameMode::UpdateTime(float DeltaSeconds)
{
	CurrentTime += DeltaSeconds;

	// Day Change
	if(CurrentTime >= ONE_DAY)
	{
		DayChange();
		UE_LOG(LogTemp, Log, TEXT("Game State - Day : %d CurrentTime : %f ProgressPercent : %f"), DayScore, CurrentTime, DayProgressPercent);
	}
	
	// 하루가 얼마나 지났는지 퍼센트로 표시
	DayProgressPercent = 100 * (CurrentTime / ONE_DAY);
	CurrentHour = CurrentTime / ONE_HOUR;
	CurrentMinute = ((60/ONE_HOUR) * static_cast<int32>(CurrentTime)) % 60;
}

float ARWGameMode::GetCurrentTime() const
{
	return CurrentTime;
}

float ARWGameMode::GetDayProgressPercent() const
{
	return DayProgressPercent;
}

int32 ARWGameMode::GetDayScore() const
{
	return DayScore;
}

void ARWGameMode::DayChange()
{
	DayScore++;
	// 24시간 초과 시 00부터 시작하도록
	CurrentTime -= ONE_DAY;
	// Spawn Animal
	UpdateMaxWolfNum();
	DayChangeSpawnAnimals();
}

// 분리
void ARWGameMode::DayChangeSpawnAnimals()
{
	for(auto& AnimalClass : AnimalClasses)
	{
		// Map 순회 , 하나의 Map
		for(int i = 0; i < AnimalMaxNumMap[AnimalClass] - AnimalCurrentNumMap[AnimalClass]; i++)
		{
			FRandomStream RandomStream;
			
			FVector SpawnLocation = FVector(100.0f + RandomStream.FRandRange(0.f, 100.f), 100.0f + RandomStream.FRandRange(0.f, 100.f), 100.0f  + RandomStream.FRandRange(0.f, 100.f));
			FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
			GetWorld()->SpawnActor<ACharacter>(AnimalClass, SpawnLocation, SpawnRotation);
		
			AnimalCurrentNumMap[AnimalClass]++;
		
			UE_LOG(LogTemp, Log, TEXT("GameMode : Spawn %s"), *AnimalClass->GetName());
		}
	}
	
}

void ARWGameMode::UpdateMaxWolfNum()
{
	// 구현 필요
}


void ARWGameMode::DecreaseCurrentAnimalNum(ARWAnimalBase* Animal)
{
	TSubclassOf<ARWAnimalBase> AnimalClass = Animal->GetClass();
	
	if(AnimalCurrentNumMap[AnimalClass] > 0)
	{
		AnimalCurrentNumMap[AnimalClass]--;
	}
}


