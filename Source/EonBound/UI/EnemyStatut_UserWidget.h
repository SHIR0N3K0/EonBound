// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyStatut_UserWidget.generated.h"

class UEonBound_AbilitySystemComponent;
class AEonBound_Character;
class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class EONBOUND_API UEnemyStatut_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_HP;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_EnemyName;
	
	UPROPERTY()
	TObjectPtr<AEonBound_Character> BindedEnemyCharacter;
	
	UPROPERTY()
	TObjectPtr<UEonBound_AbilitySystemComponent> BindedEnemyCharacterASC;
	
	virtual void NativeDestruct() override;
	
public:
	
	virtual void InitializeWidgetInfo(AEonBound_Character* Character, UEonBound_AbilitySystemComponent* ASC);
	
	void UpdateHealth();
	
	void SetName(const FString& NewName);
	
	FDelegateHandle HealthChangedHandle;
	
};
