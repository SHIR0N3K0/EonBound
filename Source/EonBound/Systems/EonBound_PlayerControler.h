// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EonBound_PlayerControler.generated.h"

class UInputAction;
class UInputMappingContext;
class ACameraActor;
class UEonBound_CombatManager;
class AEonBound_GameMode;
class AEonBound_Character;

/**
 * 
 */
UCLASS()
class EONBOUND_API AEonBound_PlayerControler : public APlayerController
{
	GENERATED_BODY()
	
public:
	AEonBound_PlayerControler();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(Transient)
	TObjectPtr<ACameraActor> CameraActor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> BasicAttackAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

private:
	void HandleBasicAttack();
	void HandleInteract();

	UPROPERTY()
	TObjectPtr<UEonBound_CombatManager> Manager;
	
	UPROPERTY()
	TObjectPtr<AEonBound_Character> CursorTarget;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetCameraActor(ACameraActor* NewCameraActor);
	
	UFUNCTION(BlueprintCallable, Category = "Camera")
	virtual ACameraActor* GetCameraActor() const;
	
	void SetManagerREF (UEonBound_CombatManager* InManager)
	{
		Manager = InManager;
	}
	
};
