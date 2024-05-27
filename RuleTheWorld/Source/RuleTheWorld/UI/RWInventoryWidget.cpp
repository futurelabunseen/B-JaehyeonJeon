// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RWInventoryWidget.h"



URWInventoryWidget::URWInventoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void URWInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	
	// Interface로 Component를 찾아내는 놀라운 방법 / TArray로 반환하니 0번째 인덱스를 반환하도록 함
	UActorComponent* InventoryComponent3 = GetOwningPlayer()->GetComponentsByInterface(URWInventoryWidgetInterface::StaticClass())[0];
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent3);

	if(InventoryWidget)
	{
		// Declare : RWInventoryInterface , Define : RWInventoryComponent
		InventoryWidget->SetUpInventoryWidget(this);
	}
}

void URWInventoryWidget::InventoryCopy()
{
	
	UActorComponent* InventoryComponent2 = GetOwningPlayer()->GetComponentsByInterface(URWInventoryWidgetInterface::StaticClass())[0];
	IRWInventoryWidgetInterface* InventoryWidget =  Cast<IRWInventoryWidgetInterface>(InventoryComponent2);

	
	// Declare : RWInventoryInterface , Define : RWInventoryComponent
	CopiedInventory = InventoryWidget->GetInventoryData();
	for(auto& a : CopiedInventory.InventoryItemNums)
	{
		UE_LOG(LogTemp, Log, TEXT("Inventory Num : %d"), a);
	}
	
		
}
