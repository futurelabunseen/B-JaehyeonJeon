// Fill out your copyright notice in the Description page of Project Settings.


#include "RWItemMeat.h"

ARWItemMeat::ARWItemMeat()
{
	ItemData = EItemData::Meat;
	bReplicates = true;
}

// 초기화된 후에 SetReplicates를 호출해야 할 경우
void ARWItemMeat::BeginPlay()
{
	Super::BeginPlay();

	// BeginPlay 이후에 SetReplicates를 호출하는 것은 괜찮습니다.
	SetReplicates(true);
}