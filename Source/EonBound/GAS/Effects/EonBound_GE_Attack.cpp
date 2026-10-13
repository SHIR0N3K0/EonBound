// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_GE_Attack.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"	

UEonBound_GE_Attack::UEonBound_GE_Attack()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
	// Attack
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetDamageAttribute();
		Modifier.ModifierOp = EGameplayModOp::Additive;
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(
			FScalableFloat(20.f)
		);
		Modifiers.Add(Modifier);
	}
}
