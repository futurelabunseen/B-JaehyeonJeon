// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Level/RWLevelScript.h"

#include "Components/DirectionalLightComponent.h"
#include "Blueprint/UserWidget.h"
#include "Game/RWGameState.h"

constexpr float DAY_PROGRESS_PERCENT_PER_MIN = 3.6f;
constexpr float MOON_LIGHT_INTENSITY = 0.015f;

ARWLevelScript::ARWLevelScript()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialzie SunLight
	SunLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("SunLight"));
	SunLight->SetAtmosphereSunLight(true);
	// Set MoonLight
	MoonLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("MoonLight"));
	MoonLight->SetIntensity(MOON_LIGHT_INTENSITY);
	MoonLight->SetMobility(EComponentMobility::Static);
	MoonLight->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));
	
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/RuleTheWorld/UI/Widget_Main.Widget_Main_C"));
	WidgetClass = WidgetClassFinder.Class;
}

void ARWLevelScript::PostNetInit()
{
	Super::PostNetInit();
}

void ARWLevelScript::BeginPlay()
{
	Super::BeginPlay();


	//AddMainWidget();
	GameState = Cast<ARWGameState>(GetWorld()->GetGameState());
}

void ARWLevelScript::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateSunLightRotation();
}

// Client Only
void ARWLevelScript::AddMainWidget_Implementation()
{
	if(WidgetClass)
	{
		MainWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
		MainWidget->AddToViewport();
	}
}

void ARWLevelScript::UpdateSunLightRotation_Implementation()
{
	// Update SunLight Rotation
	float RotatePitch = 90 + GameState->GetClientDayProgressPercent() * DAY_PROGRESS_PERCENT_PER_MIN;
	FRotator NewSunRotaion = FRotator(RotatePitch, 0, 0);
	SunLight->SetWorldRotation(NewSunRotaion);
}
