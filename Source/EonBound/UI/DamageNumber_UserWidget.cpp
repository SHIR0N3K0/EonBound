// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound/UI/DamageNumber_UserWidget.h"

#include "Components/TextBlock.h"

void UDamageNumber_UserWidget::SetDamageValue(float DamageValue, bool bIsCritical)
{
	if (TextAppearance)
	{
		PlayAnimation(TextAppearance);
	}
	
	if (IsValid(TXT_DamageNumber))
	{
		FString DamageString = FString::Printf(TEXT("%.0f"), DamageValue);
		
		if (bIsCritical && DamageValue > 0)
		{
			TXT_DamageNumber->SetColorAndOpacity(FLinearColor::Red);
			FSlateFontInfo FontInfo = TXT_DamageNumber->GetFont();
			FontInfo.Size = 42;
			TXT_DamageNumber->SetFont(FontInfo);
			TXT_DamageNumber->SetText(FText::FromString(DamageString + TEXT("!")));
			if (Crit)
			{
				PlayAnimation(Crit);
			}
		}
		else if (!bIsCritical && DamageValue > 0)
		{
			TXT_DamageNumber->SetColorAndOpacity(FSlateColor(FLinearColor::White));
			FSlateFontInfo FontInfo = TXT_DamageNumber->GetFont();
			FontInfo.Size = 32;
			TXT_DamageNumber->SetFont(FontInfo);
			TXT_DamageNumber->SetText(FText::FromString({DamageString}));
		}
		else if (DamageValue < 0)
		{
			TXT_DamageNumber->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
			FSlateFontInfo FontInfo = TXT_DamageNumber->GetFont();
			FontInfo.Size = 32;
			TXT_DamageNumber->SetFont(FontInfo);
			TXT_DamageNumber->SetText(FText::FromString({DamageString}));
		}
	}
}

void UDamageNumber_UserWidget::NativeOnAnimationFinished(const UWidgetAnimation* Animation)
{
	if (Animation == TextAppearance || Animation == Crit)
	{
		RemoveFromParent();
	}
}
