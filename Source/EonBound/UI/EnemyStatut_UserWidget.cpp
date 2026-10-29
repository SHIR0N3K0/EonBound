// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound/UI/EnemyStatut_UserWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "EonBound/Character/EonBound_Character.h"
#include "EonBound/GAS/EonBound_AbilitySystemComponent.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"

void UEnemyStatut_UserWidget::InitializeWidgetInfo(AEonBound_Character* Character, UEonBound_AbilitySystemComponent* ASC)
{
	BindedEnemyCharacter = Character;
	
	if (!IsValid(BindedEnemyCharacter))
	{
		return;
	}

	BindedEnemyCharacterASC = ASC;

	if (!BindedEnemyCharacterASC)
	{
		return;
	}
	
	// Bind to health changes
	HealthChangedHandle = BindedEnemyCharacterASC->
		GetGameplayAttributeValueChangeDelegate(UEonBound_AttributeSet::GetHealthAttribute())
			.AddWeakLambda(this, [this](const FOnAttributeChangeData& Data)
				{UpdateHealth();});
	
	if (BindedEnemyCharacter)
	{
		SetName(BindedEnemyCharacter->GetName());
		UpdateHealth();
	}
}

void UEnemyStatut_UserWidget::UpdateHealth()
{
	if (!BindedEnemyCharacterASC)
	{
		return;
	}

	float CurrentHealth = BindedEnemyCharacterASC->GetNumericAttribute(UEonBound_AttributeSet::GetHealthAttribute());
	float MaxHealth = BindedEnemyCharacterASC->GetNumericAttribute(UEonBound_AttributeSet::GetHealthMaxAttribute());

	float ProgressBarPercent = (MaxHealth > 0) ? (CurrentHealth / MaxHealth) : 0.0f;
	
	if (ProgressBar_HP)
	{
		ProgressBar_HP->SetPercent(ProgressBarPercent);
	}
}

void UEnemyStatut_UserWidget::SetName(const FString& NewName)
{
	if (TXT_EnemyName)
	{
		TXT_EnemyName->SetText(FText::FromString(NewName));
	}
}

void UEnemyStatut_UserWidget::NativeDestruct()
{
	if (IsValid(BindedEnemyCharacterASC) && HealthChangedHandle.IsValid())
	{
		BindedEnemyCharacterASC->GetGameplayAttributeValueChangeDelegate(UEonBound_AttributeSet::GetHealthAttribute())
			.Remove(HealthChangedHandle);

		HealthChangedHandle.Reset();
	}
	
	Super::NativeDestruct();
}
