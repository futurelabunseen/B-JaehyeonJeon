	// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/RWPlayerInventory.h"

#include "Object/RWInteractableActor.h"

	constexpr int InventorySize = 10;
// Sets default values
ARWPlayerInventory::ARWPlayerInventory()
{
	// Inventory Initial
	Inventory.Init(nullptr, InventorySize);
}

void ARWPlayerInventory::GetItem(ARWInteractableActor* Item)
{
	// 처음부터 Inventory를 돌며 빈 곳을 확인
	bool bIsFull = true;
	for(auto& Iter : Inventory)
	{
		if(Iter == nullptr)
		{
			Iter = Item;
			Item->Destroy();
			bIsFull = false;
			break;
		}
	}

	if(bIsFull)
	{
		UE_LOG(LogTemp, Log, TEXT("Inventory : Inventory is full!"))
	}
}

void ARWPlayerInventory::UseItem()
{
}

void ARWPlayerInventory::DeleteItem()
{
}


