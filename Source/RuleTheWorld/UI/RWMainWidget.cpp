// Fill out your copyright notice in the Description page of Project Settings.


#include "RWMainWidget.h"

#include "Character/RWCharacterPlayer.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Interface/RWCharacterWidgetInterface.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

URWMainWidget::URWMainWidget(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	bIsMinimapCompleted = false;

	MaxHunger = -1.0f;
}

void URWMainWidget::NativeConstruct()
{
	Super::NativeConstruct();


	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));
	ensure(HPProgressBar); // 있는지 검증
	HungerProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HungerBar")));
	ensure(HPProgressBar); // 있는지 검증
	
	CharacterPlayer = Cast<ARWCharacterPlayer>(GetOwningPlayerPawn());

	// 인터페이스에서 정의한 CharacterBase에서 MaxHp,Hunger 값 설정하고 Delegate 연결하는 함수를 실행
	IRWCharacterWidgetInterface* CharacterWidget = Cast<IRWCharacterWidgetInterface>(CharacterPlayer);
	if(CharacterWidget)
	{
		CharacterWidget->SetUpCharacterWidget(this);
	}
	

	// Native Construct에서 바로 실행하는 경우에 Minimap textrue를 받아오지 못함.
	// 이를 위해 임시방편으로 Timer를 통해 특정시간 이후에 실행하도록 설정
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SetMinimap, this, &URWMainWidget::SetMinimap, 0.1f, false);
}

void URWMainWidget::SetMinimap()
{
	if(!GetOwningPlayer()) { return; }
	
	// Minimap Material Instance 지정
	SetMinimapMaterial();

	// MinimapMaterial이 존재할 경우에 Bind 진행
	if(MinimapMaterialInstance)
	{
		BindImageFromMinimapMaterial();
	}
}

// Minimap Material을 
void URWMainWidget::SetMinimapMaterial()
{
	// Content 폴더 내의 메테리얼 경로
	FString MaterialPath = "/Game/RuleTheWorld/UI/M_Minimap.M_Minimap";
	
	// 메테리얼을 로드
	MinimapMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, *MaterialPath));
	
	// 메테리얼 인스턴스를 생성하고 원하는 메테리얼을 지정
	MinimapMaterialInstance = UMaterialInstanceDynamic::Create(MinimapMaterial, this);
}

void URWMainWidget::BindImageFromMinimapMaterial()
{
	if(CharacterPlayer)
	{
		// 캐릭터의 SceneCapture에서 TextureRenderTarget을 받아와 Material의 Parameter에 배정
		UTextureRenderTarget2D* MinimapTexture = CharacterPlayer->MiniMapSceneCapture->TextureTarget;
		if(MinimapTexture)
		{
			MinimapMaterialInstance->SetTextureParameterValue("Minimap", MinimapTexture);
			// 이미지 바인딩	
			MinimapImage = Cast<UImage>(GetWidgetFromName(TEXT("Minimap")));
			if(MinimapImage)
			{
				MinimapImage->SetBrushFromMaterial(MinimapMaterialInstance);
				
				// ####  원형 미니맵을 제작하고자 한 코드지만, Material을 적용하면 네모모양으로 나옴... ㅠㅠ #####
				FSlateBrush SlateBrush = MinimapImage->GetBrush();
				SlateBrush.DrawAs = ESlateBrushDrawType::RoundedBox;
				
				SlateBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::HalfHeightRadius;
				//	 ###################################################
				
				MinimapImage->SetBrush(SlateBrush);
				bIsMinimapCompleted = true;
			}
		}
	}

	if(!bIsMinimapCompleted)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SetMinimap, this, &URWMainWidget::SetMinimap, 1.0f, false);
	}
}

void URWMainWidget::UpdateHungerBar(float NewCurrentHunger)
{
	ensure(MaxHunger > 0.0f);
	if(HungerProgressBar)
	{
		HungerProgressBar->SetPercent(NewCurrentHunger / MaxHunger);
	}
}

void URWMainWidget::UpdateHPBar(float NewCurrentHP)
{
	ensure(MaxHP > 0.0f);
	if(HPProgressBar)
	{
		HPProgressBar->SetPercent(NewCurrentHP / MaxHP);
	}
}
