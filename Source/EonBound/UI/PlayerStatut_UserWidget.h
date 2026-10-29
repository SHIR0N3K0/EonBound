// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"
#include "PlayerStatut_UserWidget.generated.h"

class UEonBound_AbilitySystemComponent;
class AEonBound_Character;
class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class EONBOUND_API UPlayerStatut_UserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_HP;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_CurrentHP;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_CharacterName;
	
	UPROPERTY()
	TObjectPtr<AEonBound_Character> BindedCharacter;
	
	UPROPERTY()
	TObjectPtr<UEonBound_AbilitySystemComponent> BindedCharacterASC;
	
	void NativeDestruct() override;
	
public:
	
	virtual void SetBindedCharacter(AEonBound_Character* Character);
	
	void UpdateHealth();
	
	void SetName(const FString& NewName);
	
	FDelegateHandle HealthChangedHandle;
	

};
