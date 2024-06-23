// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RWEventWidget.h"

#include "Components/Image.h"

void URWEventWidget::SetDayEventImagVisible(uint8 bVisible)
{
	if(bVisible)
	{
		DayEventImage->SetVisibility(ESlateVisibility::Visible);	
	}
	else
	{
		DayEventImage->SetVisibility(ESlateVisibility::Hidden);	
	}
}

void URWEventWidget::SetNightEventImagVisible(uint8 bVisible)
{
	if(bVisible)
	{
		NightEventImage->SetVisibility(ESlateVisibility::Visible);	
	}
	else
	{
		NightEventImage->SetVisibility(ESlateVisibility::Hidden);	
	}
}
