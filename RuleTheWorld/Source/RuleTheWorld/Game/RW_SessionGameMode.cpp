// Fill out your copyright notice in the Description page of Project Settings.


#include "RW_SessionGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ARW_SessionGameMode::BeginPlay()
{
	Super::BeginPlay();

	
	if (SessionWidgetClass != nullptr)
	{
		SessionWidget = CreateWidget<UUserWidget>(GetWorld(), SessionWidgetClass);
		if (SessionWidget != nullptr)
		{
			SessionWidget->AddToViewport();
		}
	}

	// 마우스 커서 보이도록
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	Controller->bShowMouseCursor = true;

	// 마우스 인터페이스 모드 설정
	Controller->SetInputMode(FInputModeGameAndUI());
}

void ARW_SessionGameMode::CreateServer()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("BP_MainMap"), true, ((FString)(L"Listen")));
}

void ARW_SessionGameMode::JoinServer(FString IP)
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(*IP)); // Level 이름 대신 IP 주소.
}

void ARW_SessionGameMode::QuitGame()
{
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	if (Controller)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), Controller, EQuitPreference::Quit, false);
	}
}