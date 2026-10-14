// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EonBound_PlayerControler.generated.h"

class UInputAction;
class UInputMappingContext;

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

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> BasicAttackAction;

private:
	void HandleBasicAttack();
};
