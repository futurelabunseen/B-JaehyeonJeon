// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/RWGameMode.h"

#include "RWGameState.h"

#include "Animal/RWAnimalBase.h"


#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"

constexpr float START_TIME = 240.f; // Game Start 06:00(120)  시연을 위해 12:00로설정
constexpr float ONE_DAY = 480.f; // 8min(Real Time) = 1Day / 1min = 3Hours / 20sec = 1Hour
constexpr int ONE_HOUR = 20;
constexpr int WOLF_INCREASE_BY_DAY = 3;

// 06~18시 돼지&여우 리스폰, 18:00 ~ 24:00 늑대
constexpr float NEW_DAY_TIME = 0.f;
constexpr float MORNING_START_TIME = 120.f;
constexpr float MORNING_END_TIME = 360.f;
constexpr float EVENING_START_TIME = 360.f;
constexpr float EVENING_END_TIME = 480.f;
constexpr float INTERVAL = 40.f; // 두 시간에 한 번씩 

ARWGameMode::ARWGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	
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
	static ConstructorHelpers::FClassFinder<AGameStateBase> GameStateClassRef(TEXT("/Game/RuleTheWorld/Game/BP_RWGameState.BP_RWGameState_C"));
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
	AnimalClassMap.Add(EAnimalData::Wolf, WolfClass);
		
	static ConstructorHelpers::FClassFinder<ARWAnimalBase> PigClassRef(TEXT("/Game/RuleTheWorld/Animal/BP_Pig.BP_Pig_C"));
	if(PigClassRef.Class)
	{
		PigClass = PigClassRef.Class;
	}
	AnimalClassMap.Add(EAnimalData::Pig, PigClass);
	
	static ConstructorHelpers::FClassFinder<ARWAnimalBase> FoxClassRef(TEXT("/Game/RuleTheWorld/Animal/BP_Fox.BP_Fox_C"));
	if(FoxClassRef.Class)
	{
		FoxClass = FoxClassRef.Class;
	}
	AnimalClassMap.Add(EAnimalData::Fox, FoxClass);
	
	// Day
	CurrentTime = START_TIME;
	DayScore = 0;
	DayProgressPercent = START_TIME / ONE_DAY;

	// Animal
	// Set Initial Value
	AnimalNumMap.Add(EAnimalData::Wolf, {5, 0});
	AnimalNumMap.Add(EAnimalData::Pig, {20, 0});
	AnimalNumMap.Add(EAnimalData::Fox, {1, 0});

}

void ARWGameMode::BeginPlay()
{
	Super::BeginPlay();

	// First Day Setting
	DayChangeSpawnAnimals();
	// Set GameState
	GameState = GetWorld()->GetGameState<ARWGameState>();
}

void ARWGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateTime(DeltaSeconds);
}


// Function to spawn animals (excluding wolves) and broadcast morning event
void ARWGameMode::HandleActivateAnimal()
{
	if (CurrentTime >= MORNING_START_TIME && CurrentTime < MORNING_END_TIME)
	{
		if (FMath::Fmod(CurrentTime, INTERVAL) < 1.f) 
		{
			DayChangeSpawnAnimals(); // 늑대 제외 동물 스폰
			OnTimeReachedMorning.Broadcast(); // 아침이 되었음을 알림
		}
	}
	else
	{
		if (FMath::Fmod(CurrentTime, INTERVAL) < 1.f) 
		{
			NightWolfSpawn(); // 늑대 스폰
			OnTimeReachedNight.Broadcast(); // 저녁이 되었음을 알림
		}
	}
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
	HandleActivateAnimal();
	
	// 하루가 얼마나 지났는지 퍼센트로 표시
	DayProgressPercent = 100 * (CurrentTime / ONE_DAY);
	CurrentHour = CurrentTime / ONE_HOUR;
	CurrentMinute = ((60/ONE_HOUR) * static_cast<int32>(CurrentTime)) % 60;
	// 각 클라이언트로 뿌려주기 위함
	GameState->SetCurrentTime(CurrentTime);
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
	GameState->SetDayScore(DayScore);
	// Spawn Animal
	UpdateMaxWolfNum();
}

// 분리
void ARWGameMode::DayChangeSpawnAnimals()
{
	for(auto& AnimalClass : AnimalClassMap)
	{
		EAnimalData AnimalData = AnimalClass.Key;
		if(AnimalData == EAnimalData::Wolf) continue; // 늑대는 타 함수에서 생성
		
		for(int i = 0; i < AnimalNumMap[AnimalData][0] - AnimalNumMap[AnimalData][1]; i++) // 최대 - 현재로 스폰할 동물의 수 지정
		{
			// 현재 시간을 기반으로 시드 생성
			int32 CurrentRealTime = FDateTime::Now().GetMillisecond();

			// RandomStream 인스턴스 생성 및 난수 생성
			FRandomStream RandomStream;
			RandomStream.Initialize(CurrentRealTime);
			
			FVector SpawnLocation = FVector(100.0f + RandomStream.FRandRange(-5000.f, 5000.f), 100.0f + RandomStream.FRandRange(-5000.f, 5000.f), 1000.f);
			FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
			
			GetWorld()->SpawnActor<ACharacter>(AnimalClass.Value, SpawnLocation, SpawnRotation);
			
			AnimalNumMap[AnimalData][1]++;
			
			UE_LOG(LogTemp, Log, TEXT("스폰 위치  :  %f %f %f"), SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
			UE_LOG(LogTemp, Log, TEXT("GameMode : Spawn %s"), *AnimalClass.Value->GetName());
		}
	}
}

void ARWGameMode::NightWolfSpawn()
{
	EAnimalData AnimalData = EAnimalData::Wolf;
	// Map 순회 , 하나의 Map  
	for(int i = 0; i < AnimalNumMap[AnimalData][0] - AnimalNumMap[AnimalData][1]; i++) // 최대 - 현재로 스폰할 동물의 수 지정
	{
		// 현재 시간을 기반으로 시드 생성
		int32 CurrentRealTime = FDateTime::Now().GetMillisecond();

		// RandomStream 인스턴스 생성 및 난수 생성
		FRandomStream RandomStream;
		RandomStream.Initialize(CurrentRealTime);
			
		FVector SpawnLocation = FVector(100.0f + RandomStream.FRandRange(-5000.f, 5000.f), 100.0f + RandomStream.FRandRange(-5000.f, 5000.f), 1000.0f);
		FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
			
		GetWorld()->SpawnActor<ACharacter>(WolfClass, SpawnLocation, SpawnRotation);
			
		AnimalNumMap[AnimalData][1]++;
			
		UE_LOG(LogTemp, Log, TEXT("스폰 위치  :  %f %f %f"), SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
		UE_LOG(LogTemp, Log, TEXT("GameMode : Spawn %s"), *WolfClass->GetName());
	}
}

void ARWGameMode::UpdateMaxWolfNum() // 매일 스폰되는 늑대의 최대 수를 증가시킴
{
	AnimalNumMap[EAnimalData::Wolf][0] += WOLF_INCREASE_BY_DAY;
}

FOnTimeReached& ARWGameMode::GetOnTimeReachedMorning()
{
	return OnTimeReachedMorning;
}

FOnTimeReached& ARWGameMode::GetOnTimeReachedNight()
{
	return OnTimeReachedNight;
}

