// Fill out your copyright notice in the Description page of Project Settings.


#include "EonBound_GE_DefaultAttribute.h"
#include "EonBound/GAS/EonBound_AttributeSet.h"

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
		Modifier.Attribute = UEonBound_AttributeSet::GetSpeedAttribute();
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
	
	// BalancingCoefficient
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetBalancingCoefficientAttribute();
		Modifier.ModifierOp = EGameplayModOp::Override;
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(
			FScalableFloat(100.f)
		);

		Modifiers.Add(Modifier);
	}
	
	// CritChance
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetCritChanceAttribute();
		Modifier.ModifierOp = EGameplayModOp::Override;
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(
			FScalableFloat(5.f)
		);

		Modifiers.Add(Modifier);
	}
	
	// CritDamage
	{
		FGameplayModifierInfo Modifier;
		Modifier.Attribute = UEonBound_AttributeSet::GetCritDamageAttribute();
		Modifier.ModifierOp = EGameplayModOp::Override;
		Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(
			FScalableFloat(120.f)
		);

		Modifiers.Add(Modifier);
	}
}
