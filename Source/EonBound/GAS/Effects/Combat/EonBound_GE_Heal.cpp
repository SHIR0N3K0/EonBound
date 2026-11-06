// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound/GAS/Effects/Combat//EonBound_GE_Heal.h"

#include "EonBound/GAS/EonBound_AttributeSet.h"	
#include "GameplayTagContainer.h"

UEonBound_GE_Heal::UEonBound_GE_Heal()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
	// Heal
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetHealthAttribute();
		Modifier.ModifierOp = EGameplayModOp::Additive;
		
		FSetByCallerFloat SetByCallerMagnitude;
		SetByCallerMagnitude.DataTag =FGameplayTag::RequestGameplayTag(FName("Data.Heal"));

		Modifier.ModifierMagnitude =FGameplayEffectModifierMagnitude(SetByCallerMagnitude);
		
		Modifiers.Add(Modifier);
	}
}
