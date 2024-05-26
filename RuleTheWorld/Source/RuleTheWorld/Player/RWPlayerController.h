// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "RWPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RULETHEWORLD_API ARWPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ARWPlayerController();
	
public:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	virtual void OnPossess();

	virtual void PostNetInit() override;

// Character
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ARWCharacterPlayer> PlayerPawn;

// UI
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> HUDWidgetClass; // BP에서 지정

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<class UUserWidget> HUDWidgetInstance;
	void HUDInstancing();
	void HUDAddToViewport();
	
// Input System
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> EnableLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> RunAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> SneakingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =  Input, Meta = (AllowPrivateAccess = "True"))
	TObjectPtr<class UInputAction> FocusingAction;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EnableLook(const FInputActionValue& Value);
	void DisableLook(const FInputActionValue& Value);
	void ControllerJump(const FInputActionValue& Value);
	void ControllerStopJumping(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	void StopRunning(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);
	void Sneaking(const FInputActionValue& Value);
	void StopSneaking(const FInputActionValue& Value);
	void Focusing(const FInputActionValue& Value);
	void StopFocusing(const FInputActionValue& Value);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "True"))
	uint8 bIsEnableLook:1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "True"))
	uint8 bIsRunning:1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "True"))
	uint8 bIsSneaking:1;


	TObjectPtr<AActor> FocusingTargetActor = nullptr;
	
	FTimerHandle JumpTimerHandle;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "True"))
	uint8 bIsFocusing:1;

// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TObjectPtr<class URWInventoryComponent> InventoryComponent;
};
