// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound/UI/CombatHUD_UserWidget.h"

#include "Components/Button.h"
#include "EonBound/UI/PlayerStatut_UserWidget.h"
#include "EonBound/Character/EonBound_Character.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "EonBound/Combat/EonBound_CombatManager.h"

void UCombatHUD_UserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (IsValid(BTN_Basic))
	{
		BTN_Basic->OnClicked.AddDynamic(this, &UCombatHUD_UserWidget::OnBasicButtonClicked);
	}
	if (IsValid(BTN_Skill))
	{
		BTN_Skill->OnClicked.AddDynamic(this, &UCombatHUD_UserWidget::OnSkillButtonClicked);
	}
	if (IsValid(Overlay_Ult))
	{
		Overlay_Ult->SetVisibility(ESlateVisibility::Visible);
	}
	if (IsValid(BTN_Ult))
	{
		BTN_Ult->OnClicked.AddDynamic(this, &UCombatHUD_UserWidget::OnUltButtonClicked);
	}
}

void UCombatHUD_UserWidget::InitializePlayerStatuses(const TArray<AEonBound_Character*>& PlayerCharacters)
{
	if (!IsValid(HB_PlayerStatus) || !PlayerStatusWidgetClass)
	{
		return;
	}

	HB_PlayerStatus->ClearChildren();

	for (AEonBound_Character* Character : PlayerCharacters)
	{
		if (!IsValid(Character))
		{
			continue;
		}

		UPlayerStatut_UserWidget* StatusWidget = CreateWidget<UPlayerStatut_UserWidget>(GetOwningPlayer(),PlayerStatusWidgetClass);

		if (!IsValid(StatusWidget))
		{
			continue;
		}

		StatusWidget->SetBindedCharacter(Character);
		AddPlayerStatusWidget(StatusWidget);
	}
}

void UCombatHUD_UserWidget::AddPlayerStatusWidget(UUserWidget* PlayerStatusWidget)
{
	if (!IsValid(HB_PlayerStatus) || !IsValid(PlayerStatusWidget))
	{
		UE_LOG(LogTemp, Warning,TEXT("AddPlayerStatusWidget: Invalid widget or HorizontalBox."));
		return;
	}

	UHorizontalBoxSlot* StatusSlot = HB_PlayerStatus->AddChildToHorizontalBox(PlayerStatusWidget);
	
	if (StatusSlot)
	{
		StatusSlot->SetPadding(FMargin(0.f, 0.f,30.f,0.f));
		StatusSlot->SetVerticalAlignment(VAlign_Center);
		StatusSlot->SetHorizontalAlignment(HAlign_Center);
	}
}

void UCombatHUD_UserWidget::OnBasicButtonClicked()
{
	if (IsValid(CombatManagerRef))
	{
		AEonBound_Character* EnemyTargeted = CombatManagerRef->SelectedTarget;
		if (EnemyTargeted != nullptr)
		{
			CombatManagerRef->RequestPlayerBasicAttack(EnemyTargeted);
		}
	}
}

void UCombatHUD_UserWidget::OnSkillButtonClicked()
{
	if (!IsValid(CombatManagerRef))
	{
		return;
	}

	CombatManagerRef->ToggleSkillTargeting();
}

void UCombatHUD_UserWidget::OnUltButtonClicked()
{
	if (!IsValid(CombatManagerRef))
	{
		return;
	}

	CombatManagerRef->ToggleUltimateTargeting();
}

void UCombatHUD_UserWidget::SetUltButtonEnabled(bool bEnabled)
{
	if (IsValid(BTN_Ult))
	{
		BTN_Ult->SetIsEnabled(bEnabled);
	}
}

void UCombatHUD_UserWidget::SetButtonVisibility(ECombatHUDState State, bool bIsVisible)
{
	switch (State)
	{
	case ECombatHUDState::Basic:
		if (IsValid(Overlay_Basic))
		{
			Overlay_Basic->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		}	
		break;
	case ECombatHUDState::Skill:
		if (IsValid(Overlay_Skill))
		{
			Overlay_Skill->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		}
		break;
	case ECombatHUDState::Ult:
		if (IsValid(Overlay_Ult))
		{
			Overlay_Ult->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		}
		break;
	}
}

void UCombatHUD_UserWidget::SetSkillButtonEnabled(bool bEnabled)
{
	if (IsValid(BTN_Skill))
	{
		BTN_Skill->SetIsEnabled(bEnabled);
	}
}

void UCombatHUD_UserWidget::SetSkillCooldown(int32 RemainingTurns)
{
	if (!IsValid(TXT_CoolDownSkill))
	{
		return;
	}

	if (RemainingTurns > 0)
	{
		TXT_CoolDownSkill->SetText(FText::AsNumber(RemainingTurns));
		TXT_CoolDownSkill->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		TXT_CoolDownSkill->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCombatHUD_UserWidget::SetEnergyDisplay(float Current, float Max)
{
	if (IsValid(TXT_EnergyValue))
	{
		TXT_EnergyValue->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), Current, Max)));
	}

	const float Percent = Max > 0.f ? FMath::Clamp(Current / Max, 0.f, 1.f) : 0.f;
	OnEnergyUpdated(Current, Max, Percent);
}

void UCombatHUD_UserWidget::OnEnergyUpdated(float Current, float Max, float Percent)
{
}
