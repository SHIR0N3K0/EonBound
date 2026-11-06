// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumber_UserWidget.generated.h"


class UTextBlock;
/**
 * 
 */
UCLASS()
class EONBOUND_API UDamageNumber_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_DamageNumber;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> TextAppearance;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Crit;
	
public:
	
	void SetDamageValue(float DamageValue, bool bIsCritical = false);
	
	virtual void NativeOnAnimationFinished(const UWidgetAnimation* Animation);
	
};
