// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RWGameState.h"

#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"



constexpr float START_TIME_CLIENT = 120.f; // Game Start 06:00 
constexpr float ONE_DAY_CLIENT = 480.f; // 8min(Real Time) = 1Day / 1min = 3Hours / 20sec = 1Hour
constexpr int ONE_HOUR_CLIENT = 20;

ARWGameState::ARWGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	// Initialize
	DayScore = 0;
	CurrentTime = START_TIME_CLIENT;
	DayProgressPercent = 0.f;
	CurrentHour = 0;
	CurrentMinute = 0;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	
	bReplicates = true;
}

void ARWGameState::BeginPlay()
{
	Super::BeginPlay();
	PlaySound(DayBGM);
}

void ARWGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CalcTimeVariable();
}

void ARWGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARWGameState, CurrentTime);	
	DOREPLIFETIME(ARWGameState, DayScore);
}

// Tick마다 호출
void ARWGameState::CalcTimeVariable()
{
	DayProgressPercent = 100 * (CurrentTime / ONE_DAY_CLIENT);
	CurrentHour = CurrentTime / ONE_HOUR_CLIENT;
	CurrentMinute = ((60/ONE_HOUR_CLIENT) * static_cast<int32>(CurrentTime)) % 60;

	if(CurrentTime == 120.f) // 06:00 DaySound
	{
		PlaySound(DayBGM);
		UE_LOG(LogTemp, Log, TEXT("PlayMusic"));
	}
	else if(CurrentTime == 360.f) // 18:00 Night Sound
	{
		PlaySound(NightBGM);
		UE_LOG(LogTemp, Log, TEXT("PlayMusic"));
	}
}

// GameMode에서 Data를 넘겨줄 때 사용
void ARWGameState::SetCurrentTime(float InputCurrnetTime)
{
	CurrentTime = InputCurrnetTime;
}
// GameMode에서 Data를 넘겨줄 때 사용
void ARWGameState::SetDayScore(int32 InputDayScore)
{
	DayScore = InputDayScore;
}

float ARWGameState::GetClientCurrentTime() const
{
	return CurrentTime;
}

float ARWGameState::GetClientDayProgressPercent() const
{
	return DayProgressPercent;
}

int32 ARWGameState::GetClientDayScore() const
{
	return DayScore;
}

void ARWGameState::PlaySound(USoundWave* SoundWave)
{
	AudioComponent->Stop();
	AudioComponent->SetSound(SoundWave);
	AudioComponent->Play();
}


