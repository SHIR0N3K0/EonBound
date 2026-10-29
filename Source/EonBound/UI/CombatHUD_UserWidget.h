// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatHUD_UserWidget.generated.h"

class UOverlay;
class UEonBound_CombatManager;
class UButton;
class UHorizontalBox;
class UPlayerStatut_UserWidget;
class AEonBound_Character;
/**
 * 
 */

enum class ECombatHUDState : uint8
{
	Basic,
	Skill,
	Ult
};

UCLASS()
class EONBOUND_API UCombatHUD_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HB_PlayerStatus;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat HUD")
	TSubclassOf<UPlayerStatut_UserWidget> PlayerStatusWidgetClass;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Basic;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_Basic;
	
	virtual void NativeConstruct() override;
	
public:
	
	
	void InitializePlayerStatuses(const TArray<AEonBound_Character*>& PlayerCharacters);
	
	UFUNCTION()
	void AddPlayerStatusWidget(UUserWidget* PlayerStatusWidget);
	
	UFUNCTION()
	void OnBasicButtonClicked();
	
	UPROPERTY()
	TObjectPtr<UEonBound_CombatManager> CombatManagerRef;
	
	void SetButtonVisibility(ECombatHUDState State, bool bIsVisible);
};
