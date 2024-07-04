// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Inventory/RWInventoryComponent.h"

#include "EnhancedInputComponent.h"
#include "Character/RWCharacterPlayer.h"
#include "Character/Stat/RWCharacterStatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Interface/RWCollisionedItemInterface.h"
#include "Object/RWInteractableActor.h"
#include "Object/RWItemData.h"
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

	// ItemData Map
	SetItemDataAssetMap();
}

void URWInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetIsReplicated(true);
	
	BindAction();
	InitializeInterface();
	
	APlayerController* Controller = Cast<APlayerController>(GetOwner());
	if(Controller->IsLocalController())
	{
		InventoryWidgetInstancing();
		if(InventoryWidgetInstance)
		{
			InventoryWidgetInstance->AddToViewport();
			InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


void URWInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URWInventoryComponent, Inventory);
	DOREPLIFETIME(URWInventoryComponent, bIsProcessingPickUp);
	
}

void URWInventoryComponent::PickUp()
{
	if (CharacterInterface->GetIsItemInBound())
	{
		ARWInteractableActor* CollisionedItem = CharacterInterface->GetCollisionedItem();
		if (CollisionedItem)
		{
			EItemData ItemData = CollisionedItem->ItemData;
			ServerRPCGetItem(ItemData);
		}
		else
		{
			OnPickUpComplete();
		}
	}
	else
	{
		OnPickUpComplete();
	}
}

bool URWInventoryComponent::ServerRPCGetItem_Validate(EItemData ItemData)
{
	/*// Client에서 PickUp을 요청한 Item과 서버에서 대상으로 하고 있는 아이템이 같은지 확인
	ARWInteractableActor* CollisionedItem = CharacterInterface->GetCollisionedItem();
	if (CollisionedItem->ItemData != ItemData)
	{
		return false;
	}
	*/

	return true;
}

void URWInventoryComponent::ServerRPCGetItem_Implementation(EItemData ItemData)
{
	if(bIsProcessingPickUp)
	{
		UE_LOG(LogTemp, Log, TEXT("픽업 거부됨"));
		return;
	}

	
	bIsProcessingPickUp = true;
	if (CharacterInterface->GetIsItemInBound())
	{
		// 해당 아이템 파괴
		ARWInteractableActor* CollisionedItem = CharacterInterface->GetCollisionedItem();
		if (CollisionedItem)
		{
			// Client에서 PickUp을 요청한 Item과 서버에서 대상으로 하고 있는 아이템이 같은지 확인
			if (CollisionedItem->ItemData != ItemData) 
			{
				return ;
			}
			
			UE_LOG(LogTemp, Log, TEXT("인벤토리에 넣어서 삭제됨"));
			CollisionedItem->Destroy();
        
			// 동일 아이템이 있는 위치를 찾고 수 증가
			int32 ItemIndex = GetItemIndex(ItemData);
			
			// None인 자리에 새로 아이템을 넣는 경우에 새로 입력받은 Item Data를 넣어줌
			if (Inventory.InventoryItemSubjects[ItemIndex] == EItemData::None)
			{
				Inventory.InventoryItemSubjects[ItemIndex] = ItemData;
			}
			// 해당 Index의 값 증가 
			AddInventoryItemNums(ItemIndex);
		}
	}
	OnPickUpComplete();
}

void URWInventoryComponent::OnPickUpComplete()
{
	bIsProcessingPickUp = false;
}


void URWInventoryComponent::InitializeInterface()
{
	// Interface 설정
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
	if(InventoryWidgetInstance)
	{
		// Inventory Widget의 시각화 여부 조절
		if(InventoryWidgetInstance->IsVisible())
		{
			InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
			
			// 마우스 커서 보이도록
			APlayerController* Controller = GetWorld()->GetFirstPlayerController();
			Controller->bShowMouseCursor = false;

			// 마우스 인터페이스 모드 설정
			Controller->SetInputMode(FInputModeGameOnly());
		}
		else
		{
			OnRepInventory.Broadcast(); // Inventory를 열 떄, Delegate로 Widget이 inventory 정보를 최신화 하도록 함
			
			InventoryWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			
			// 마우스 커서 보이도록
			APlayerController* Controller = GetWorld()->GetFirstPlayerController();
			Controller->bShowMouseCursor = true;

			// 마우스 인터페이스 모드 설정
			Controller->SetInputMode(FInputModeGameAndUI());
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

TMap<EItemData, TObjectPtr<URWItemData>> URWInventoryComponent::GetItemDataAssetMap()
{
	return ItemDataAssetMap;
}

void URWInventoryComponent::SetItemDataAssetMap()
{
	// 데이터 에셋 로드 및 매핑
	static ConstructorHelpers::FObjectFinder<URWItemData> DataAssetNone(TEXT("/Game/RuleTheWorld/Object/BP_ItemData_None.BP_ItemData_None"));
	if (DataAssetNone.Succeeded())
	{
		ItemDataAssetMap.Add(EItemData::None, DataAssetNone.Object);
	}

	static ConstructorHelpers::FObjectFinder<URWItemData> DataAssetMeat(TEXT("/Game/RuleTheWorld/Object/BP_ItemData_Meat.BP_ItemData_Meat"));
	if (DataAssetMeat.Succeeded())
	{
		ItemDataAssetMap.Add(EItemData::Meat, DataAssetMeat.Object);
	}

	static ConstructorHelpers::FObjectFinder<URWItemData> DataAssetPlant(TEXT("/Game/RuleTheWorld/Object/BP_ItemData_Plant.BP_ItemData_Plant"));
	if (DataAssetPlant.Succeeded())
	{
		ItemDataAssetMap.Add(EItemData::Plant, DataAssetPlant.Object);
	}
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

void URWInventoryComponent::UseItem(int32 ItemIndex)
{
	// 클라이언트에서 한 번 검사 
	if(Inventory.InventoryItemNums[ItemIndex] < 1 || Inventory.InventoryItemSubjects[ItemIndex] == EItemData::None)
	{
		return;
	}
	ServerRPCUseItem(ItemIndex);
	
	if(GetOwner()->HasAuthority()) // 서버도 인벤토리 최신화
	{
		OnRepInventory.Broadcast();
	}
}

void URWInventoryComponent::ServerRPCUseItem_Implementation(int32 ItemIndex)
{
	// 서버에서 한 번 더 검사
	if(Inventory.InventoryItemNums[ItemIndex] < 1 || Inventory.InventoryItemSubjects[ItemIndex] == EItemData::None)
	{
		return;
	}
	
	// 아이템 효과 적용
	APlayerController* Controller = Cast<APlayerController>(GetOwner());
	UActorComponent* ActorComponent = Controller->GetPawn()->GetComponentByClass(URWCharacterStatComponent::StaticClass());
	URWCharacterStatComponent* StatComponent = Cast<URWCharacterStatComponent>(ActorComponent);
	
	EItemData ItemSubject = Inventory.InventoryItemSubjects[ItemIndex];
	URWItemData* ItemData = ItemDataAssetMap[ItemSubject];
	if(ItemData)
	{
		// 아이템 힐링
		float HealingAmount = ItemData->HPIncreaseAmount;
		UE_LOG(LogTemp, Log, TEXT(" Healing  : %f "), HealingAmount);
	
		if(HealingAmount > 0)
		{
			StatComponent->HealHP(HealingAmount);	
		}

		// 허기 회복
		float RelieveAmount = ItemData->HungerDecreaseAmount;
		UE_LOG(LogTemp, Log, TEXT(" RelieveAmount  : %f "), RelieveAmount);
		if(RelieveAmount > 0)
		{
			StatComponent->RelieveHunger(RelieveAmount);
		}

		// 사용한 아이템의 수를 감소
		Inventory.InventoryItemNums[ItemIndex]--;
		if(Inventory.InventoryItemNums[ItemIndex] == 0)
		{
			// 해당 아이템을 소진한 경우 None으로 변경
			Inventory.InventoryItemSubjects[ItemIndex] = EItemData::None;
		}
	}
	
}

void URWInventoryComponent::DeleteItem(int32 ItemIndex)
{
	// 클라이언트에서 한 번 검사 
	if(Inventory.InventoryItemNums[ItemIndex] < 1 || Inventory.InventoryItemSubjects[ItemIndex] == EItemData::None)
	{
		return;
	}
	ServerRPCDeleteItem(ItemIndex);
}

void URWInventoryComponent::ServerRPCDeleteItem_Implementation(int32 ItemIndex)
{
	// 서버에서 한 번 더 검사
	if(Inventory.InventoryItemNums[ItemIndex] < 1 || Inventory.InventoryItemSubjects[ItemIndex] == EItemData::None)
	{
		return;
	}

	Inventory.InventoryItemNums[ItemIndex] = 0;
	Inventory.InventoryItemSubjects[ItemIndex] = EItemData::None;
}

void URWInventoryComponent::SetUpInventoryWidget(URWInventoryWidget* InventoryWidget)
{
	// Delegate 등록
	OnRepInventory.AddUObject(InventoryWidget, &URWInventoryWidget::InventoryCopy);
}


