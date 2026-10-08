// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Effects/EonBound_GE_DefaultAttribute.h"
#include "GAS/EonBound_AttributeSet.h"

UEonBound_GE_DefaultAttribute::UEonBound_GE_DefaultAttribute()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
	// MaxHealth
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetHealthMaxAttribute();
		Modifier.ModifierOp = EGameplayModOp::Override;
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(
			FScalableFloat(100.f)
		);
		Modifiers.Add(Modifier);
	}
	
	// Health
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetHealthAttribute();
		Modifier.ModifierOp = EGameplayModOp::Override;
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(
			FScalableFloat(100.f)
		);

		Modifiers.Add(Modifier);
	}
	
	// Attack
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetAttackAttribute();
		Modifier.ModifierOp = EGameplayModOp::Override;
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(
			FScalableFloat(10.f)
		);

		Modifiers.Add(Modifier);
	}
	
	// Defense
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetDefenseAttribute();
		Modifier.ModifierOp = EGameplayModOp::Override;
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(
			FScalableFloat(7.f)
		);

		Modifiers.Add(Modifier);
	}
	
	// Speed
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetDefenseAttribute();
		Modifier.ModifierOp = EGameplayModOp::Override;
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(
			FScalableFloat(75.f)
		);

		Modifiers.Add(Modifier);
	}
	
	// Energy
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetEnergyAttribute();
		Modifier.ModifierOp = EGameplayModOp::Override;
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(
			FScalableFloat(75.f)
		);

		Modifiers.Add(Modifier);
	}
}
