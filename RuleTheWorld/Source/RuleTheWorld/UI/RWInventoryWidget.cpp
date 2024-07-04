// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RWInventoryWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Interface/RWItemDataInterface.h"
#include "Interface/RWInventoryWidgetInterface.h"
#include "Object/RWItemData.h"


URWInventoryWidget::URWInventoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void URWInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	InventoryImages = {InventoryImage0, InventoryImage1, InventoryImage2, InventoryImage3, InventoryImage4,
		InventoryImage5, InventoryImage6, InventoryImage7, InventoryImage8, InventoryImage9};

	ItemNumTexts = {ItemNumText0, ItemNumText1, ItemNumText2, ItemNumText3, ItemNumText4, ItemNumText5, ItemNumText6, ItemNumText7, ItemNumText8, ItemNumText9};
	
	// Interface로 Component를 찾아내는 놀라운 방법 / TArray로 반환하니 0번째 인덱스를 반환하도록 함
	InventoryComponent = GetOwningPlayer()->GetComponentsByInterface(URWInventoryWidgetInterface::StaticClass())[0];
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);

	CopiedInventory = InventoryWidget->GetInventoryData();
	
	if(InventoryWidget)
	{
		// Declare : RWInventoryInterface , Define : RWInventoryComponent
		InventoryWidget->SetUpInventoryWidget(this);
	}

	//Item Use Button Binding
	BindItemUseBtn();
}

void URWInventoryWidget::InventoryCopy()
{
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);
	
	// Declare : RWInventoryInterface , Define : RWInventoryComponent
	CopiedInventory = InventoryWidget->GetInventoryData();
	BindTexturesFromInterface(); // 인벤토리 받아올 때, Texture 설정도 같이 
}


void URWInventoryWidget::SetImageTexture(UImage* ImageWidget, UTexture2D* NewTexture)
{
	if (ImageWidget && NewTexture)
	{
		ImageWidget->SetBrushFromTexture(NewTexture);
	}
}


void URWInventoryWidget::SetItemText(UTextBlock* TextBlock, const uint8& ItemNum)
{
	if(TextBlock && ItemNum)
	{
		FString ItemNumString = FString::Printf(TEXT("%d"), ItemNum);
		TextBlock->SetText(FText::FromString(ItemNumString));
	}
}

void URWInventoryWidget::BindItemUseBtn()
{
	if (ItemUseBtn0)
	{
		ItemUseBtn0->OnClicked.AddDynamic(this, &URWInventoryWidget::UseItemHandler0);
	}
	if (ItemUseBtn1)
	{
		ItemUseBtn1->OnClicked.AddDynamic(this, &URWInventoryWidget::UseItemHandler1);
	}
	if (ItemUseBtn2)
	{
		ItemUseBtn2->OnClicked.AddDynamic(this, &URWInventoryWidget::UseItemHandler2);
	}
	if (ItemUseBtn3)
	{
		ItemUseBtn3->OnClicked.AddDynamic(this, &URWInventoryWidget::UseItemHandler3);
	}
	if (ItemUseBtn4)
	{
		ItemUseBtn4->OnClicked.AddDynamic(this, &URWInventoryWidget::UseItemHandler4);
	}
	if (ItemUseBtn5)
	{
		ItemUseBtn5->OnClicked.AddDynamic(this, &URWInventoryWidget::UseItemHandler5);
	}
	if (ItemUseBtn6)
	{
		ItemUseBtn6->OnClicked.AddDynamic(this, &URWInventoryWidget::UseItemHandler6);
	}
	if (ItemUseBtn7)
	{
		ItemUseBtn0->OnClicked.AddDynamic(this, &URWInventoryWidget::UseItemHandler7);
	}
	if (ItemUseBtn8)
	{
		ItemUseBtn8->OnClicked.AddDynamic(this, &URWInventoryWidget::UseItemHandler8);
	}
	if (ItemUseBtn9)
	{
		ItemUseBtn9->OnClicked.AddDynamic(this, &URWInventoryWidget::UseItemHandler9);
	}
	
}

void URWInventoryWidget::UseItemHandler0()
{
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);
	if(InventoryWidget)
	{
		InventoryWidget->UseItem(0);
	}
}

void URWInventoryWidget::UseItemHandler1()
{
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);
	if(InventoryWidget)
	{
		InventoryWidget->UseItem(1);
	}
}

void URWInventoryWidget::UseItemHandler2()
{
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);
	if(InventoryWidget)
	{
		InventoryWidget->UseItem(2);
	}
}

void URWInventoryWidget::UseItemHandler3()
{
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);
	if(InventoryWidget)
	{
		InventoryWidget->UseItem(3);
	}
}

void URWInventoryWidget::UseItemHandler4()
{
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);
	if(InventoryWidget)
	{
		InventoryWidget->UseItem(4);
	}
}

void URWInventoryWidget::UseItemHandler5()
{
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);
	if(InventoryWidget)
	{
		InventoryWidget->UseItem(5);
	}
}

void URWInventoryWidget::UseItemHandler6()
{
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);
	if(InventoryWidget)
	{
		InventoryWidget->UseItem(6);
	}
}

void URWInventoryWidget::UseItemHandler7()
{
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);
	if(InventoryWidget)
	{
		InventoryWidget->UseItem(7);
	}
}

void URWInventoryWidget::UseItemHandler8()
{
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);
	if(InventoryWidget)
	{
		InventoryWidget->UseItem(8);
	}
}

void URWInventoryWidget::UseItemHandler9()
{
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent);
	if(InventoryWidget)
	{
		InventoryWidget->UseItem(9);
	}
}


void URWInventoryWidget::BindTexturesFromInterface()
{
	IRWItemDataInterface* ItemDataInterface = Cast<IRWItemDataInterface>(InventoryComponent);
	const TMap<EItemData, TObjectPtr<URWItemData>>& ItemDataMap = ItemDataInterface->GetItemDataAssetMap();

	int32 Index = 0;
	for(auto& InventoryImage : InventoryImages)
	{
		if (Index >= InventoryImages.Num())
		{
			break;
		}
		// Image에 텍스쳐 설정
		URWItemData* ItemData = ItemDataMap[CopiedInventory.InventoryItemSubjects[Index]];
		SetImageTexture(InventoryImage, ItemData->ItemTexture);
		SetItemText(ItemNumTexts[Index], CopiedInventory.InventoryItemNums[Index]);
		Index++;
	}
}

