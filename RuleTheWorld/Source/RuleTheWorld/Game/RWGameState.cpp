// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/RWGameState.h"
#include "Game/RWGameMode.h"
#include "Net/UnrealNetwork.h"



constexpr float START_TIME_CLIENT = 100.f; // Game Start 06:00 
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

	bReplicates = true;
}

void ARWGameState::BeginPlay()
{
	Super::BeginPlay();

	// Server Code
	if(HasAuthority())
	{
		RWGameMode = Cast<ARWGameMode>(GetWorld()->GetAuthGameMode());
	}
}

void ARWGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// ServerCode
	if(HasAuthority())
	{
		UpdateTimeFromGameMode();
	}
}

void ARWGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARWGameState, CurrentTime);	
}

// ServerOnly
void ARWGameState::UpdateTimeFromGameMode()
{
	// Current Time 변수 값을 GameMode에서 받아옴 
	CurrentTime = RWGameMode->GetCurrentTime();
	// Day Change
	if(CurrentTime >= ONE_DAY_CLIENT)
	{
		DayScore++;
		CurrentTime -= ONE_DAY_CLIENT;
		UE_LOG(LogTemp, Log, TEXT("Game State - Day : %d CurrentTime : %f ProgressPercent : %f"), DayScore, CurrentTime, DayProgressPercent);
	}
	DayProgressPercent = 100 * (CurrentTime / ONE_DAY_CLIENT);
	CurrentHour = CurrentTime / ONE_HOUR_CLIENT;
	CurrentMinute = ((60/ONE_HOUR_CLIENT) * static_cast<int32>(CurrentTime)) % 60;
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

/*
void ARWGameState::UpdateTime(float DeltaSeconds)
{
	CurrentTime += DeltaSeconds;
	// Day Change
	if(CurrentTime >= ONE_DAY_CLIENT)
	{
		DayScore++;
		CurrentTime -= ONE_DAY_CLIENT;
		UE_LOG(LogTemp, Log, TEXT("Game State - Day : %d CurrentTime : %f ProgressPercent : %f"), DayScore, CurrentTime, DayProgressPercent);
	}
	// 하루가 얼마나 지났는지 퍼센트로 표시
	DayProgressPercent = 100 * (CurrentTime / ONE_DAY_CLIENT);
	CurrentHour = CurrentTime / ONE_HOUR_CLIENT;
	CurrentMinute = ((60/ONE_HOUR_CLIENT) * static_cast<int32>(CurrentTime)) % 60;
}
*/

