// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Inventory/RWInventoryComponent.h"

#include "EnhancedInputComponent.h"
#include "Character/RWCharacterPlayer.h"
#include "Net/UnrealNetwork.h"
#include "..\..\Interface\RWCollisionedItemInterface.h"
#include "Object/RWInteractableActor.h"
#include "UI/RWInventoryWidget.h"


constexpr int INVENTORY_SIZE = 10;

// Sets default values for this component's properties
URWInventoryComponent::URWInventoryComponent()
{
	static ConstructorHelpers::FObjectFinder<UInputAction> InventoryWidgetOnScreenActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_InventoryWidget.IA_InventoryWidget'"));
	if(nullptr != InventoryWidgetOnScreenActionRef.Object)
	{
		InventoryWidgetOnScreenAction = InventoryWidgetOnScreenActionRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionPickUpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/RuleTheWorld/Input/Action/IA_PickUp.IA_PickUp'"));
	if(nullptr != InputActionPickUpRef.Object)
	{
		PickUpAction = InputActionPickUpRef.Object;
	}

	Inventory.InventoryItemSubjects.Init(EItemData::None, INVENTORY_SIZE);
	Inventory.InventoryItemNums.Init(0, INVENTORY_SIZE);

	SetIsReplicated(true);
}

void URWInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	BindAction();
	InitializeInterface();
	
	APlayerController* Controller = Cast<APlayerController>(GetOwner());
	if(Controller->IsLocalController())
	{
		InventoryWidgetInstancing();
	}
}


void URWInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URWInventoryComponent, Inventory);
}

void URWInventoryComponent::PickUp()
{
	if(CharacterInterface->bIsItemInBound)
	{
		EItemData ItemData = CharacterInterface->CollisionedItem->ItemData;
		ServerRPCGetItem(ItemData);
	}


}

bool URWInventoryComponent::ServerRPCGetItem_Validate(EItemData ItemData)
{
	// Client에서 PickUP을 요청한 Item과 서버에서 대상으로 하고있는 아이템이 같은지 확인
	if(CharacterInterface->CollisionedItem->ItemData != ItemData)
	{
		
		return false;
	}
	
	return true;
}

void URWInventoryComponent::ServerRPCGetItem_Implementation(EItemData ItemData)
{
	// 해당 아이템 파괴
	if(CharacterInterface->bIsItemInBound)
	{
		CharacterInterface->CollisionedItem->Destroy();
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

void URWInventoryComponent::InitializeInterface()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	IRWCollisionedItemInterface* II = Cast<IRWCollisionedItemInterface>(PlayerController->GetCharacter());
	
	CharacterInterface.SetInterface(II);
	CharacterInterface.SetObject(PlayerController->GetCharacter());
	
	FTimerHandle InterfaceInitializeTimerHandle;
	if(!CharacterInterface) // 설정되지 않았을 경우
	{
		GetWorld()->GetTimerManager().SetTimer(
			InterfaceInitializeTimerHandle,
			this,
			&URWInventoryComponent::InitializeInterface,
			0.5f,
			false 
		);
	}
}

void URWInventoryComponent::SetInventoryWidgetVisibility()
{
	UE_LOG(LogTemp, Log, TEXT("하나"));
	if(InventoryWidgetInstance)
	{
		UE_LOG(LogTemp, Log, TEXT("둘"));

		// Inventory Widget이 Viewport에 없으면 넣고, 있으면 뺌
		if(InventoryWidgetInstance->IsInViewport())
		{
			UE_LOG(LogTemp, Log, TEXT("셋"));

			InventoryWidgetInstance->RemoveFromParent();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("넷"));

			InventoryWidgetInstance->AddToViewport();
			UE_LOG(LogTemp, Log, TEXT("다섯"));
		}
	}
}

void URWInventoryComponent::InventoryWidgetInstancing()
{
	if(InventoryWidgetClass != nullptr)
	{
		InventoryWidgetInstance = CreateWidget<UUserWidget>(Cast<APlayerController>(GetOwner()), InventoryWidgetClass);
	}
}

void URWInventoryComponent::BindAction()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController)
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Bind PickUp Action
			EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Triggered, this, &URWInventoryComponent::PickUp);
			EnhancedInputComponent->BindAction(InventoryWidgetOnScreenAction, ETriggerEvent::Triggered, this, &URWInventoryComponent::SetInventoryWidgetVisibility);
		}
	}
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

FInventory URWInventoryComponent::GetInventoryData()
{
	return Inventory;
}

void URWInventoryComponent::OnRep_CopiedInventoryToWidget()
{
	UE_LOG(LogTemp, Log, TEXT("Inventory Rep"));
	OnRepInventory.Broadcast();
}

void URWInventoryComponent::SetUpInventoryWidget(URWInventoryWidget* InventoryWidget)
{
	// Delegate 등록;
	OnRepInventory.AddUObject(InventoryWidget, &URWInventoryWidget::InventoryCopy);
}


