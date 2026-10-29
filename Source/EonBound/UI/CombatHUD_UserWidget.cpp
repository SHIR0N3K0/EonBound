// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound/UI/CombatHUD_UserWidget.h"

#include "Components/Button.h"
#include "EonBound/UI/PlayerStatut_UserWidget.h"
#include "EonBound/Character/EonBound_Character.h"
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
		StatusSlot->SetPadding(FMargin(0.f, 0.f,10.f,0.f));
		StatusSlot->SetVerticalAlignment(VAlign_Fill);
		StatusSlot->SetHorizontalAlignment(HAlign_Fill);
	}
}

void UCombatHUD_UserWidget::OnBasicButtonClicked()
{
	if (IsValid(CombatManagerRef))
	{
		AEonBound_Character* EnemyTargeted = CombatManagerRef->SelectedEnemyTarget;
		if (EnemyTargeted != nullptr)
		{
			CombatManagerRef->RequestPlayerBasicAttack(EnemyTargeted);
		}
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
		// Handle Skill button visibility
		break;
	case ECombatHUDState::Ult:
		// Handle Ultimate button visibility
		break;
	}
}
