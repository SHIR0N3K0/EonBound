// Fill out your copyright notice in the Description page of Project Settings.

 
#include "EonBound/UI/PlayerStatut_UserWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "EonBound/Character/EonBound_Character.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"


void UPlayerStatut_UserWidget::SetBindedCharacter(AEonBound_Character* Character)
{
	BindedCharacter = Character;
	
	if (!IsValid(BindedCharacter))
	{
		return;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(BindedCharacter);

	if (!AbilitySystemInterface)
	{
		return;
	}

	UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		return;
	}

	BindedCharacterASC = Cast<UEonBound_AbilitySystemComponent>(ASC);
	if (!IsValid(BindedCharacterASC))
	{
		return;
	}
	
	// Bind to health changes
	HealthChangedHandle = BindedCharacterASC->
		GetGameplayAttributeValueChangeDelegate(UEonBound_AttributeSet::GetHealthAttribute())
			.AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
				{UpdateHealth();});
	
	if (BindedCharacter)
	{
		SetName(BindedCharacter->GetName());
		UpdateHealth();
	}
}

void UPlayerStatut_UserWidget::UpdateHealth()
{
	if (!BindedCharacterASC)
	{
		return;
	}

	float CurrentHealth = BindedCharacterASC->GetNumericAttribute(UEonBound_AttributeSet::GetHealthAttribute());
	float MaxHealth = BindedCharacterASC->GetNumericAttribute(UEonBound_AttributeSet::GetHealthMaxAttribute());

	float ProgressBarPercent = (MaxHealth > 0) ? (CurrentHealth / MaxHealth) : 0.0f;
	
	if (ProgressBar_HP)
	{
		ProgressBar_HP->SetPercent(ProgressBarPercent);
		
		if (ProgressBarPercent <= 0.3f)
		{
			ProgressBar_HP->SetFillColorAndOpacity(FLinearColor::Red);
		}
		else if (ProgressBarPercent <= 0.6f)
		{
			ProgressBar_HP->SetFillColorAndOpacity(FLinearColor::Yellow);
		}
		else
		{
			ProgressBar_HP->SetFillColorAndOpacity(FLinearColor::Green);
		}
	}
	
	if (TXT_CurrentHP)
	{
		FString HealthValue = FString::FromInt(CurrentHealth);
		TXT_CurrentHP->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), CurrentHealth)));
	}
}

void UPlayerStatut_UserWidget::SetName(const FString& NewName)
{
	if (TXT_CharacterName)
	{
		TXT_CharacterName->SetText(FText::FromString(NewName));
	}
}

void UPlayerStatut_UserWidget::NativeDestruct()
{
	if (IsValid(BindedCharacterASC) && HealthChangedHandle.IsValid())
	{
		BindedCharacterASC->GetGameplayAttributeValueChangeDelegate(UEonBound_AttributeSet::GetHealthAttribute())
			.Remove(HealthChangedHandle);

		HealthChangedHandle.Reset();
	}
	Super::NativeDestruct();
}
