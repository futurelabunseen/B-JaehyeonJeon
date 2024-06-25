// Fill out your copyright notice in the Description page of Project Settings.


#include "RWOpenSessionWidget.h"


#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Game/RW_SessionGameMode.h"

void URWOpenSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CreateServerBtn->OnClicked.AddDynamic(this, &URWOpenSessionWidget::CreateServerHandler);
	JoinServerBtn->OnClicked.AddDynamic(this, &URWOpenSessionWidget::JoinServerHandler);
	QuitGameBtn->OnClicked.AddDynamic(this, &URWOpenSessionWidget::QuitGameHandler);
}

void URWOpenSessionWidget::CreateServerHandler()
{
	ARW_SessionGameMode* GameMode = Cast<ARW_SessionGameMode>(GetWorld()->GetAuthGameMode());
	if(GameMode)
	{
		GameMode->CreateServer();
	}
}

void URWOpenSessionWidget::JoinServerHandler()
{
	ARW_SessionGameMode* GameMode = Cast<ARW_SessionGameMode>(GetWorld()->GetAuthGameMode());
	if(GameMode)
	{
		if (IPAddressTextBox)
		{
			FString IPAddress = IPAddressTextBox->GetText().ToString();
			GameMode->JoinServer(IPAddress);
		}
	}
}

void URWOpenSessionWidget::QuitGameHandler()
{
	ARW_SessionGameMode* GameMode = Cast<ARW_SessionGameMode>(GetWorld()->GetAuthGameMode());
	if(GameMode)
	{
		GameMode->QuitGame();
	}
}