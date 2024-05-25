// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Inventory/RWInventoryComponent.h"

#include "EnhancedInputComponent.h"
#include "Character/RWCharacterPlayer.h"
#include "Net/UnrealNetwork.h"
#include "Interface/RWInventoryInterface.h"
#include "Object/RWInteractableActor.h"


constexpr int INVENTORY_SIZE = 10;

// Sets default values for this component's properties
URWInventoryComponent::URWInventoryComponent()
{
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionPickUpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_PickUp.IA_PickUp'"));
	if(nullptr != InputActionPickUpRef.Object)
	{
		PickUpAction = InputActionPickUpRef.Object;
	}

	Inventory.InventoryItemSubjects.Init(EItemData::None, INVENTORY_SIZE);
	Inventory.InventoryItemNums.Init(0, INVENTORY_SIZE);

	//bReplicates = true;
	SetIsReplicated(true);
}

void URWInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController)
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Bind PickUp Action
			EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Triggered, this, &URWInventoryComponent::PickUp);
		}
	}
}


void URWInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URWInventoryComponent, Inventory);
}

void URWInventoryComponent::PickUp()
{
	IRWInventoryInterface* CharacterFromInterface = Cast<IRWInventoryInterface>(PlayerController->GetCharacter());
	if(CharacterFromInterface->bIsItemInBound)
	{
		EItemData ItemData = CharacterFromInterface->CollisionedItem->ItemData;
		ServerRPCGetItem(ItemData);
	}
}

bool URWInventoryComponent::ServerRPCGetItem_Validate(EItemData ItemData)
{
	// Client에서 PickUP을 요청한 Item과 서버에서 대상으로 하고있는 아이템이 같은지 확인
	IRWInventoryInterface* CharacterFromInterface = Cast<IRWInventoryInterface>(PlayerController->GetCharacter());
	if(CharacterFromInterface->CollisionedItem->ItemData != ItemData)
	{
		return false;
	}
	
	return true;
}

void URWInventoryComponent::ServerRPCGetItem_Implementation(EItemData ItemData)
{
	// 해당 아이템 파괴
	IRWInventoryInterface* CharacterFromInterface = Cast<IRWInventoryInterface>(PlayerController->GetCharacter());
	if(CharacterFromInterface->bIsItemInBound)
	{
		CharacterFromInterface->CollisionedItem->Destroy();
	}
	// 동일 아이템이 있는 위치를 찾고 수 증가
	int32 ItemIndex = GetItemIndex(ItemData);
	AddInventoryItemNums(ItemIndex);	
}

void URWInventoryComponent::UseItem()
{
}

void URWInventoryComponent::DeleteItem()
{
}

int32 URWInventoryComponent::GetItemIndex(EItemData ItemData)
{
	int32 FindIndex;
	FindIndex = Inventory.InventoryItemSubjects.Find(ItemData);
	
	if(FindIndex == INDEX_NONE) // 해당 ItemData가 Inventory에 없는 경우
	{
		FindIndex = Inventory.InventoryItemSubjects.Find(EItemData::None);
		if(FindIndex == INDEX_NONE)
		{
			return -1; // Inventory가 가득 참
		}
	}

	return FindIndex;
}

void URWInventoryComponent::AddInventoryItemNums(int32 ItemIndex)
{
	if(ItemIndex == -1) // -1은 Inventory가 가득 찬 경우
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is full!"));
		return ;	
	}
	if(Inventory.InventoryItemNums[ItemIndex] >= 256) // uint8의 최대 값
	{
		UE_LOG(LogTemp, Warning, TEXT("This item is reached MAX NUM!"));
		return;	
	}
	
	Inventory.InventoryItemNums[ItemIndex]++;
}

