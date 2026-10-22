// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_GE_Attack.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"	
#include "GameplayTagContainer.h"
#include "EonBound/GAS/Effects/EonBound_MMC_AttackDamage.h"

UEonBound_GE_Attack::UEonBound_GE_Attack()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
	// Attack
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetDamageAttribute();
		Modifier.ModifierOp = EGameplayModOp::Additive;
		
		FSetByCallerFloat SetByCallerMagnitude;
		SetByCallerMagnitude.DataTag =FGameplayTag::RequestGameplayTag(FName("Data.Damage"));

		Modifier.ModifierMagnitude =FGameplayEffectModifierMagnitude(SetByCallerMagnitude);
		
		Modifiers.Add(Modifier);
	}
}
